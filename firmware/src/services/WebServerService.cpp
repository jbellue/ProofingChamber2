#include "WebServerService.h"
#include "../AppContext.h"
#include "../ITemperatureController.h"
#include "../IInputManager.h"
#include "../StorageConstants.h"
#include "../services/IStorage.h"
#include "../DebugUtils.h"
#include "../screens/controllers/ProofingController.h"
#include "../screens/controllers/CoolingController.h"
#include "../MenuActions.h"
#include "../screens/Menu.h"
#include "../ScreensManager.h"
#include <ArduinoJson.h>

namespace services {

WebServerService::WebServerService(AppContext* ctx)
    : _ctx(ctx), _server(nullptr), _ws(nullptr) {
    // Don't create AsyncWebServer here - delays port 80 allocation
    // until after WiFi captive portal completes
}

WebServerService::~WebServerService() {
    if (_ws) {
        delete _ws;
        _ws = nullptr;
    }
    if (_server) {
        delete _server;
        _server = nullptr;
    }
}

void WebServerService::begin() {
    // Create the server now, after WiFi connection is established
    // This avoids port conflict with WiFiManager captive portal
    if (!_server) {
        DEBUG_PRINTLN("Creating AsyncWebServer on port 80...");
        _server = new AsyncWebServer(80);
        if (!_server) {
            DEBUG_PRINTLN("ERROR: Failed to create AsyncWebServer!");
            return;
        }
    }
    
    DEBUG_PRINTLN("Setting up WebSocket...");
    setupWebSocket();
    
    DEBUG_PRINTLN("Setting up web server routes...");
    setupRoutes();
    
    DEBUG_PRINTLN("Starting web server...");
    _server->begin();
    DEBUG_PRINTLN("✓ Web server started successfully on port 80");
    DEBUG_PRINTLN("  Access via IP address or http://proofingchamber.local");
}

void WebServerService::update() {
    // Clean up WebSocket clients periodically
    if (_ws) {
        _ws->cleanupClients();
    }
    
    // Broadcast screen state every 500ms
    static unsigned long lastBroadcast = 0;
    unsigned long now = millis();
    if (now - lastBroadcast >= 500) {
        lastBroadcast = now;
        broadcastScreenState();
    }
}

void WebServerService::setupWebSocket() {
    if (!_server) return;
    
    _ws = new AsyncWebSocket("/ws");
    _ws->onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, 
                       AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->onWebSocketEvent(server, client, type, arg, data, len);
    });
    _server->addHandler(_ws);
}

void WebServerService::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                                       AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            DEBUG_PRINT("WebSocket client #");
            DEBUG_PRINT(client->id());
            DEBUG_PRINTLN(" connected");
            break;
            
        case WS_EVT_DISCONNECT:
            DEBUG_PRINT("WebSocket client #");
            DEBUG_PRINT(client->id());
            DEBUG_PRINTLN(" disconnected");
            break;
            
        case WS_EVT_ERROR:
            DEBUG_PRINT("WebSocket error from client #");
            DEBUG_PRINTLN(client->id());
            break;
            
        case WS_EVT_DATA:
            // We don't expect data from clients for display mirroring
            break;
            
        case WS_EVT_PONG:
            // Pong received
            break;
    }
}

void WebServerService::broadcastScreenState() {
    if (!_ws || _ws->count() == 0) return;
    
    // Build JSON with current screen state
    StaticJsonDocument<512> doc;
    
    // Get current temperature
    if (_ctx->tempController) {
        float temp = _ctx->tempController->getTemperature();
        doc["temperature"] = temp;
    }
    
    // Get current mode
    TemperatureMode mode = _ctx->tempController->getMode();
    const char* modeStr = "off";
    if (mode == TemperatureMode::HEATING) modeStr = "heating";
    else if (mode == TemperatureMode::COOLING) modeStr = "cooling";
    doc["mode"] = modeStr;
    
    // Get screen name
    if (_ctx->screens && _ctx->screens->getActiveScreen()) {
        doc["screenName"] = _ctx->screens->getActiveScreen()->getScreenName();
    }
    
    // Get proofing state
    if (_ctx->proofingController && _ctx->proofingController->isActive()) {
        time_t startTime = _ctx->proofingController->getStartTime();
        if (startTime > 0) {
            time_t now = _ctx->clock->now();
            doc["proofingElapsedSeconds"] = (int)(now - startTime);
        }
    }
    
    // Get cooling state
    if (_ctx->coolingController && _ctx->coolingController->isActive()) {
        time_t endTime = _ctx->coolingController->getEndTime();
        if (endTime > 0) {
            time_t now = _ctx->clock->now();
            doc["coolingRemainingSeconds"] = (int)(endTime - now);
            doc["coolingEndTime"] = endTime;
        }
    }
    
    String output;
    serializeJson(doc, output);
    _ws->textAll(output);
}

void WebServerService::setupRoutes() {
    if (!_server) return;  // Safety check
    
    // Serve the main web page
    _server->on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetRoot(request);
    });
    
    // API endpoints
    _server->on("/api/settings", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetSettings(request);
    });
    
    _server->on("/api/mode", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleSetMode(request);
    });
    
    _server->on("/api/settings", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleSetSettings(request);
    });
    
    _server->on("/api/cooling/schedule", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleScheduleCooling(request);
    });
    
    // New virtual input endpoints
    _server->on("/api/input/button", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleInjectButton(request);
    });
    
    _server->on("/api/input/encoder", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleInjectEncoder(request);
    });
    
    // Quick action endpoints
    _server->on("/api/action/proof-now", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleProofNow(request);
    });
    
    _server->on("/api/action/proof-at", HTTP_POST, [this](AsyncWebServerRequest* request) {}, nullptr, 
        [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            handleProofAt(request, data, len);
        });
    
    _server->on("/api/action/proof-in", HTTP_POST, [this](AsyncWebServerRequest* request) {}, nullptr,
        [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            handleProofIn(request, data, len);
        });
    
    _server->on("/api/action/stop", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleStopOperation(request);
    });
}

void WebServerService::handleGetRoot(AsyncWebServerRequest* request) {
    request->send(200, "text/html", getWebPageHtml());
}

void WebServerService::handleGetSettings(AsyncWebServerRequest* request) {
    JsonDocument doc;
    
    if (_ctx->storage) {
        JsonObject heating = doc["heating"].to<JsonObject>();
        heating["lowerLimit"] = _ctx->storage->getInt(
            storage::keys::HOT_LOWER_LIMIT_KEY, 
            storage::defaults::HOT_LOWER_LIMIT_DEFAULT
        );
        heating["upperLimit"] = _ctx->storage->getInt(
            storage::keys::HOT_UPPER_LIMIT_KEY, 
            storage::defaults::HOT_UPPER_LIMIT_DEFAULT
        );
        
        JsonObject cooling = doc["cooling"].to<JsonObject>();
        cooling["lowerLimit"] = _ctx->storage->getInt(
            storage::keys::COLD_LOWER_LIMIT_KEY, 
            storage::defaults::COLD_LOWER_LIMIT_DEFAULT
        );
        cooling["upperLimit"] = _ctx->storage->getInt(
            storage::keys::COLD_UPPER_LIMIT_KEY, 
            storage::defaults::COLD_UPPER_LIMIT_DEFAULT
        );
    }
    
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void WebServerService::handleSetMode(AsyncWebServerRequest* request) {
    if (!request->hasParam("mode", true)) {
        request->send(400, "application/json", "{\"error\":\"Missing 'mode' parameter\"}");
        return;
    }
    
    String mode = request->getParam("mode", true)->value();
    
    if (!_ctx->menuActions || !_ctx->screens || !_ctx->menu) {
        request->send(500, "application/json", "{\"error\":\"Menu system not available\"}");
        return;
    }
    
    if (mode == "heating") {
        // Use menu action to start proofing (heating mode)
        _ctx->menuActions->proofNowAction();
        request->send(200, "application/json", "{\"status\":\"ok\",\"mode\":\"heating\",\"message\":\"Started proofing via menu action\"}");
    } else if (mode == "cooling") {
        // Cooling requires time configuration, so we can't directly trigger it
        // For now, return an informative error
        request->send(400, "application/json", "{\"error\":\"Cooling mode requires time configuration. Use physical interface or schedule via dedicated endpoint.\"}");
    } else if (mode == "off") {
        // Navigate back to menu which stops current operation
        BaseController* currentScreen = _ctx->screens->getActiveScreen();
        if (currentScreen != _ctx->menu) {
            // Turn off temperature control
            if (_ctx->tempController) {
                _ctx->tempController->setMode(ITemperatureController::OFF);
            }
            // Return to menu
            _ctx->screens->setActiveScreen(_ctx->menu);
            request->send(200, "application/json", "{\"status\":\"ok\",\"mode\":\"off\",\"message\":\"Returned to menu\"}");
        } else {
            // Already at menu
            request->send(200, "application/json", "{\"status\":\"ok\",\"mode\":\"off\",\"message\":\"Already at menu\"}");
        }
    } else {
        request->send(400, "application/json", "{\"error\":\"Invalid mode. Use 'heating', 'cooling', or 'off'\"}");
    }
}

void WebServerService::handleSetSettings(AsyncWebServerRequest* request) {
    if (!_ctx->storage) {
        request->send(500, "application/json", "{\"error\":\"Storage not available\"}");
        return;
    }
    
    bool updated = false;
    JsonDocument doc;
    
    // Handle heating settings
    if (request->hasParam("heating_lower", true)) {
        int value = request->getParam("heating_lower", true)->value().toInt();
        // Validate temperature is reasonable (-50 to 100°C)
        if (value >= -50 && value <= 100) {
            _ctx->storage->setInt(storage::keys::HOT_LOWER_LIMIT_KEY, value);
            updated = true;
        }
    }
    if (request->hasParam("heating_upper", true)) {
        int value = request->getParam("heating_upper", true)->value().toInt();
        // Validate temperature is reasonable (-50 to 100°C)
        if (value >= -50 && value <= 100) {
            _ctx->storage->setInt(storage::keys::HOT_UPPER_LIMIT_KEY, value);
            updated = true;
        }
    }
    
    // Handle cooling settings
    if (request->hasParam("cooling_lower", true)) {
        int value = request->getParam("cooling_lower", true)->value().toInt();
        // Validate temperature is reasonable (-50 to 100°C)
        if (value >= -50 && value <= 100) {
            _ctx->storage->setInt(storage::keys::COLD_LOWER_LIMIT_KEY, value);
            updated = true;
        }
    }
    if (request->hasParam("cooling_upper", true)) {
        int value = request->getParam("cooling_upper", true)->value().toInt();
        // Validate temperature is reasonable (-50 to 100°C)
        if (value >= -50 && value <= 100) {
            _ctx->storage->setInt(storage::keys::COLD_UPPER_LIMIT_KEY, value);
            updated = true;
        }
    }
    
    if (updated) {
        // If we're currently in a mode, reload the settings
        if (_ctx->tempController && _ctx->tempController->getMode() != ITemperatureController::OFF) {
            ITemperatureController::Mode currentMode = _ctx->tempController->getMode();
            _ctx->tempController->setMode(ITemperatureController::OFF);
            _ctx->tempController->setMode(currentMode);
        }
        doc["status"] = "ok";
        doc["message"] = "Settings updated";
    } else {
        doc["status"] = "error";
        doc["message"] = "No parameters provided";
    }
    
    String response;
    serializeJson(doc, response);
    request->send(updated ? 200 : 400, "application/json", response);
}

void WebServerService::handleScheduleCooling(AsyncWebServerRequest* request) {
    // This is a placeholder for future implementation
    // The full cooling schedule logic would require more integration with the screen controllers
    request->send(501, "application/json", "{\"error\":\"Cooling schedule not yet implemented via web interface\"}");
}

// Virtual input injection handlers
void WebServerService::handleInjectButton(AsyncWebServerRequest* request) {
    if (!_ctx->input) {
        request->send(500, "application/json", "{\"error\":\"Input manager not available\"}");
        return;
    }
    
    // Inject a button press
    _ctx->input->injectButtonPress();
    request->send(200, "application/json", "{\"status\":\"ok\",\"action\":\"button_press\"}");
}

void WebServerService::handleInjectEncoder(AsyncWebServerRequest* request) {
    if (!request->hasParam("steps", true)) {
        request->send(400, "application/json", "{\"error\":\"Missing 'steps' parameter\"}");
        return;
    }
    
    if (!_ctx->input) {
        request->send(500, "application/json", "{\"error\":\"Input manager not available\"}");
        return;
    }
    
    int steps = request->getParam("steps", true)->value().toInt();
    
    // Validate steps are reasonable (-10 to 10)
    if (steps < -10 || steps > 10) {
        request->send(400, "application/json", "{\"error\":\"Steps must be between -10 and 10\"}");
        return;
    }
    
    // Inject encoder steps
    _ctx->input->injectEncoderSteps(steps);
    
    JsonDocument doc;
    doc["status"] = "ok";
    doc["action"] = "encoder_turn";
    doc["steps"] = steps;
    
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void WebServerService::handleProofNow(AsyncWebServerRequest* request) {
    // Start proofing directly without navigation simulation
    if (!_ctx->proofingController) {
        request->send(500, "application/json", "{\"error\":\"Proofing controller not available\"}");
        return;
    }
    
    _ctx->proofingController->startProofing();
    request->send(200, "application/json", "{\"success\":true,\"message\":\"Proofing started\"}");
}

void WebServerService::handleProofAt(AsyncWebServerRequest* request, uint8_t* data, size_t len) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data, len);
    
    if (error || !doc["hour"].is<int>() || !doc["minute"].is<int>()) {
        request->send(400, "application/json", "{\"error\":\"Invalid JSON or missing hour/minute\"}");
        return;
    }
    
    int hour = doc["hour"];
    int minute = doc["minute"];
    
    // Calculate target time
    struct tm targetTime;
    getLocalTime(&targetTime);
    targetTime.tm_hour = hour;
    targetTime.tm_min = minute;
    targetTime.tm_sec = 0;
    time_t targetTimestamp = mktime(&targetTime);
    
    // If the time is in the past today, schedule for tomorrow
    struct tm now;
    getLocalTime(&now);
    time_t nowTimestamp = mktime(&now);
    if (targetTimestamp <= nowTimestamp) {
        targetTime.tm_mday += 1;
        targetTimestamp = mktime(&targetTime);
    }
    
    // Calculate delay in seconds
    int delaySeconds = difftime(targetTimestamp, nowTimestamp);
    
    // Start cooling (delay mode) that will end at the target time
    if (!_ctx->coolingController) {
        request->send(500, "application/json", "{\"error\":\"Cooling controller not available\"}");
        return;
    }
    
    _ctx->coolingController->startCooling(targetTimestamp);
    
    JsonDocument response;
    response["success"] = true;
    response["message"] = "Proofing scheduled at " + String(hour) + ":" + String(minute);
    response["delaySeconds"] = delaySeconds;
    
    String responseStr;
    serializeJson(response, responseStr);
    request->send(200, "application/json", responseStr);
}

void WebServerService::handleProofIn(AsyncWebServerRequest* request, uint8_t* data, size_t len) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data, len);
    
    if (error || !doc["hours"].is<float>()) {
        request->send(400, "application/json", "{\"error\":\"Invalid JSON or missing hours\"}");
        return;
    }
    
    float hours = doc["hours"];
    
    if (!_ctx->coolingController) {
        request->send(500, "application/json", "{\"error\":\"Cooling controller not available\"}");
        return;
    }
    
    // Start cooling with delay
    _ctx->coolingController->startCoolingWithDelay((int)hours);
    
    JsonDocument response;
    response["success"] = true;
    response["message"] = "Proofing will start in " + String(hours) + " hours";
    
    String responseStr;
    serializeJson(response, responseStr);
    request->send(200, "application/json", responseStr);
}

void WebServerService::handleStopOperation(AsyncWebServerRequest* request) {
    // Stop operation directly without navigation
    bool wasActive = false;
    
    if (_ctx->proofingController && _ctx->proofingController->isActive()) {
        _ctx->proofingController->stopProofing();
        wasActive = true;
    }
    
    if (_ctx->coolingController && _ctx->coolingController->isActive()) {
        _ctx->coolingController->stopCooling();
        wasActive = true;
    }
    
    if (wasActive) {
        request->send(200, "application/json", "{\"success\":true,\"message\":\"Operation stopped\"}");
    } else {
        request->send(200, "application/json", "{\"success\":true,\"message\":\"No active operation to stop\"}");
    }
}

String WebServerService::getWebPageHtml() {
    return R"html(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Proofing Chamber Control</title>
    <link rel="icon" href="data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 100 100'><text y='.9em' font-size='90'>🍞</text></svg>">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
            color: #333;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        .header {
            text-align: center;
            color: white;
            margin-bottom: 30px;
        }
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.2);
        }
        .card {
            background: white;
            border-radius: 12px;
            padding: 25px;
            margin-bottom: 20px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
        }
        .card h2 {
            font-size: 1.5em;
            margin-bottom: 20px;
            color: #667eea;
            border-bottom: 2px solid #f0f0f0;
            padding-bottom: 10px;
        }
        .status-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
            margin-bottom: 15px;
        }
        .status-item {
            background: #f8f9fa;
            padding: 15px;
            border-radius: 8px;
            text-align: center;
        }
        .status-label {
            font-size: 0.85em;
            color: #666;
            margin-bottom: 5px;
        }
        .status-value {
            font-size: 1.8em;
            font-weight: bold;
            color: #667eea;
        }
        .temperature {
            font-size: 3em !important;
            color: #764ba2;
        }
        .mode-buttons {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
            gap: 10px;
            margin-bottom: 15px;
        }
        .btn {
            padding: 15px 25px;
            border: none;
            border-radius: 8px;
            font-size: 1em;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(0,0,0,0.2);
        }
        .btn-heating {
            background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
            color: white;
        }
        .btn-cooling {
            background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%);
            color: white;
        }
        .btn-off {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        .btn-primary {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        .btn.active {
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.3);
            transform: scale(1.05);
        }
        .settings-group {
            margin-bottom: 25px;
        }
        .settings-group h3 {
            font-size: 1.2em;
            color: #764ba2;
            margin-bottom: 15px;
        }
        .input-group {
            display: flex;
            align-items: center;
            margin-bottom: 12px;
        }
        .input-group label {
            flex: 1;
            font-weight: 500;
            color: #555;
        }
        .input-group input {
            width: 100px;
            padding: 10px;
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            font-size: 1em;
            transition: border-color 0.3s ease;
        }
        .input-group input:focus {
            outline: none;
            border-color: #667eea;
        }
        .indicator {
            display: inline-block;
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-left: 8px;
            animation: pulse 2s infinite;
        }
        .indicator.heating {
            background: #f5576c;
        }
        .indicator.cooling {
            background: #00f2fe;
        }
        .indicator.off {
            background: #999;
        }
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        .alert {
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 15px;
            display: none;
        }
        .alert.success {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .alert.error {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        .footer {
            text-align: center;
            color: white;
            margin-top: 30px;
            opacity: 0.9;
        }
        @media (max-width: 600px) {
            .header h1 {
                font-size: 1.8em;
            }
            .status-value {
                font-size: 1.4em;
            }
            .temperature {
                font-size: 2.2em !important;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🍞 Proofing Chamber</h1>
            <p>Temperature Control System</p>
        </div>

        <div class="card">
            <h2>Current Status</h2>
            <div id="statusAlert" class="alert"></div>
            <div class="status-grid">
                <div class="status-item">
                    <div class="status-label">Temperature</div>
                    <div class="status-value temperature" id="temperature">--</div>
                </div>
                <div class="status-item">
                    <div class="status-label">Mode</div>
                    <div class="status-value" id="mode">
                        <span id="modeText">--</span>
                        <span id="modeIndicator" class="indicator off"></span>
                    </div>
                </div>
            </div>
            <div id="timingInfo" style="margin-top: 15px; padding: 10px; background: #f8f9fa; border-radius: 6px; display: none;">
                <div id="proofingTime" style="margin-bottom: 5px;"></div>
                <div id="coolingTime"></div>
            </div>
        </div>

        <div class="card">
            <h2>Screen Display</h2>
            <div style="background: #000; padding: 10px; border-radius: 8px; text-align: center;">
                <canvas id="displayCanvas" width="128" height="64" style="border: 1px solid #333; background: #000; image-rendering: pixelated; width: 100%; max-width: 512px; height: auto;"></canvas>
                <div id="wsStatus" style="margin-top: 8px; font-size: 0.85em; color: #666;">Connecting to display...</div>
            </div>
        </div>

        <div class="card">
            <h2>Quick Actions</h2>
            <p style="color: #666; margin-bottom: 15px; font-size: 0.9em;">
                Direct control buttons for common operations
            </p>
            <div class="mode-buttons">
                <button class="btn" style="background: #28a745;" onclick="startProofingNow()">
                    🔥 Proof Now
                </button>
                <button class="btn" style="background: #17a2b8;" onclick="showScheduleProofing()">
                    🕐 Schedule Proof
                </button>
                <button class="btn" style="background: #dc3545;" onclick="stopOperation()" id="stopBtn">
                    ⏹️ Stop
                </button>
            </div>
            
            <!-- Schedule Proofing Form -->
            <div id="scheduleForm" style="display: none; margin-top: 15px; padding: 15px; background: #f8f9fa; border-radius: 8px;">
                <h3 style="margin-top: 0;">Schedule Proofing</h3>
                <div class="input-group">
                    <label>Proof at specific time:</label>
                    <input type="time" id="proofAtTime" style="padding: 8px; border-radius: 4px; border: 1px solid #ddd; width: 100%;">
                </div>
                <div style="text-align: center; margin: 10px 0; color: #666;">OR</div>
                <div class="input-group">
                    <label>Proof in (hours):</label>
                    <input type="number" id="proofInHours" min="0" max="24" step="0.5" placeholder="e.g., 2.5" style="padding: 8px; border-radius: 4px; border: 1px solid #ddd; width: 100%;">
                </div>
                <div class="mode-buttons" style="margin-top: 15px;">
                    <button class="btn btn-primary" onclick="scheduleProofing()">✓ Schedule</button>
                    <button class="btn" style="background: #6c757d;" onclick="hideScheduleProofing()">Cancel</button>
                </div>
            </div>
        </div>

        <details style="margin-top: 20px;">
            <summary style="cursor: pointer; padding: 15px; background: #f8f9fa; border-radius: 8px; margin-bottom: 10px;">
                <strong>🎛️ Advanced: Virtual Controls</strong>
            </summary>
            <div class="card" style="margin-top: 0;">
                <p style="color: #666; margin-bottom: 15px; font-size: 0.9em;">
                    These buttons simulate physical device navigation (for advanced users)
                </p>
                <div class="mode-buttons">
                    <button class="btn btn-primary" onclick="encoderUp()">
                        ⬆️ Up
                    </button>
                    <button class="btn btn-primary" onclick="encoderDown()">
                        ⬇️ Down
                    </button>
                    <button class="btn btn-primary" onclick="pressButton()">
                        ✓ Select
                    </button>
                </div>
            </div>
        </details>

        <div class="card">
            <h2>Temperature Settings</h2>
            <div id="settingsAlert" class="alert"></div>
            
            <div class="settings-group">
                <h3>🔥 Heating Mode</h3>
                <div class="input-group">
                    <label>Lower Limit (°C):</label>
                    <input type="number" id="heatingLower" value="23" min="-50" max="100">
                </div>
                <div class="input-group">
                    <label>Upper Limit (°C):</label>
                    <input type="number" id="heatingUpper" value="32" min="-50" max="100">
                </div>
            </div>
            
            <div class="settings-group">
                <h3>❄️ Cooling Mode</h3>
                <div class="input-group">
                    <label>Lower Limit (°C):</label>
                    <input type="number" id="coolingLower" value="2" min="-50" max="100">
                </div>
                <div class="input-group">
                    <label>Upper Limit (°C):</label>
                    <input type="number" id="coolingUpper" value="7" min="-50" max="100">
                </div>
            </div>
            
            <button class="btn btn-primary" onclick="saveSettings()">
                💾 Save Settings
            </button>
        </div>

        <div class="footer">
            <p>Proofing Chamber Web Interface v1.0</p>
        </div>
    </div>

    <script>
        let currentMode = 'off';
        let ws = null;
        let canvas = null;
        let ctx = null;
        
        // Initialize WebSocket for display mirroring
        function initWebSocket() {
            canvas = document.getElementById('displayCanvas');
            ctx = canvas.getContext('2d');
            
            // Set up canvas for OLED-like display
            ctx.fillStyle = '#000';
            ctx.fillRect(0, 0, 128, 64);
            
            // Connect to WebSocket
            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const wsUrl = `${protocol}//${window.location.host}/ws`;
            
            ws = new WebSocket(wsUrl);
            
            ws.onopen = () => {
                document.getElementById('wsStatus').textContent = '✓ Display connected';
                document.getElementById('wsStatus').style.color = '#28a745';
            };
            
            ws.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    // Update status display from WebSocket data
                    if (data.temperature !== undefined) {
                        document.getElementById('temperature').textContent = 
                            data.temperature !== null ? data.temperature.toFixed(1) + '°C' : '--';
                    }
                    
                    if (data.mode) {
                        currentMode = data.mode;
                        const modeText = currentMode.charAt(0).toUpperCase() + currentMode.slice(1);
                        document.getElementById('modeText').textContent = modeText;
                        const indicator = document.getElementById('modeIndicator');
                        indicator.className = `indicator ${currentMode}`;
                    }
                    
                    // Update timing information
                    const timingInfo = document.getElementById('timingInfo');
                    const proofingTime = document.getElementById('proofingTime');
                    const coolingTime = document.getElementById('coolingTime');
                    let hasTimingInfo = false;
                    
                    if (data.proofingElapsedSeconds !== undefined) {
                        proofingTime.innerHTML = `<strong>⏱️ Proofing:</strong> ${formatTime(data.proofingElapsedSeconds)} elapsed`;
                        proofingTime.style.display = 'block';
                        hasTimingInfo = true;
                    } else {
                        proofingTime.style.display = 'none';
                    }
                    
                    if (data.coolingRemainingSeconds !== undefined) {
                        const remaining = Math.max(0, data.coolingRemainingSeconds);
                        coolingTime.innerHTML = `<strong>🕐 Cooling:</strong> ${formatTime(remaining)} until proofing starts`;
                        if (data.coolingEndTime) {
                            coolingTime.innerHTML += `<br><small>Starts at: ${formatDateTime(data.coolingEndTime)}</small>`;
                        }
                        coolingTime.style.display = 'block';
                        hasTimingInfo = true;
                    } else {
                        coolingTime.style.display = 'none';
                    }
                    
                    timingInfo.style.display = hasTimingInfo ? 'block' : 'none';
                    
                    // Also update screen name if provided
                    if (data.screenName) {
                        console.log('Current screen:', data.screenName);
                    }
                } catch (e) {
                    console.error('WebSocket message error:', e);
                }
            };
            
            ws.onerror = (error) => {
                document.getElementById('wsStatus').textContent = '✗ Display connection error';
                document.getElementById('wsStatus').style.color = '#dc3545';
            };
            
            ws.onclose = () => {
                document.getElementById('wsStatus').textContent = 'Reconnecting to display...';
                document.getElementById('wsStatus').style.color = '#ffc107';
                // Reconnect after 2 seconds
                setTimeout(initWebSocket, 2000);
            };
        }
        
        // Handle display commands from ESP32
        function handleDisplayCommand(cmd) {
            if (!ctx) return;
            
            switch (cmd.cmd) {
                case 'clear':
                    // Clear display (black)
                    ctx.fillStyle = '#000';
                    ctx.fillRect(0, 0, 128, 64);
                    break;
                    
                case 'text':
                    // Draw text at position
                    // OLED uses white on black
                    ctx.fillStyle = '#fff';
                    ctx.font = '10px monospace';
                    ctx.fillText(cmd.text, cmd.x, cmd.y);
                    break;
                    
                case 'render':
                    // Display update complete - refresh happens automatically
                    break;
                    
                default:
                    console.log('Unknown display command:', cmd);
            }
        }

        function showAlert(elementId, message, type) {
            const alert = document.getElementById(elementId);
            alert.className = `alert ${type}`;
            alert.textContent = message;
            alert.style.display = 'block';
            setTimeout(() => {
                alert.style.display = 'none';
            }, 3000);
        }

        function formatTime(seconds) {
            const hrs = Math.floor(seconds / 3600);
            const mins = Math.floor((seconds % 3600) / 60);
            const secs = seconds % 60;
            if (hrs > 0) {
                return `${hrs}h ${mins}m ${secs}s`;
            } else if (mins > 0) {
                return `${mins}m ${secs}s`;
            } else {
                return `${secs}s`;
            }
        }

        function formatDateTime(timestamp) {
            const date = new Date(timestamp * 1000);
            return date.toLocaleString();
        }

        // Status updates now come via WebSocket, no polling needed

        async function loadSettings() {
            try {
                const response = await fetch('/api/settings');
                const data = await response.json();
                
                if (data.heating) {
                    document.getElementById('heatingLower').value = data.heating.lowerLimit;
                    document.getElementById('heatingUpper').value = data.heating.upperLimit;
                }
                if (data.cooling) {
                    document.getElementById('coolingLower').value = data.cooling.lowerLimit;
                    document.getElementById('coolingUpper').value = data.cooling.upperLimit;
                }
            } catch (error) {
                console.error('Failed to load settings:', error);
            }
        }

        // Virtual control functions
        async function encoderUp() {
            try {
                const formData = new FormData();
                formData.append('steps', '-1');  // Negative = up/counter-clockwise
                
                const response = await fetch('/api/input/encoder', {
                    method: 'POST',
                    body: formData
                });
                
                if (response.ok) {
                    setTimeout(updateStatus, 100);  // Quick refresh
                } else {
                    const error = await response.json();
                    showAlert('statusAlert', error.error || 'Failed to send input', 'error');
                }
            } catch (error) {
                showAlert('statusAlert', 'Failed to communicate with device', 'error');
            }
        }

        async function encoderDown() {
            try {
                const formData = new FormData();
                formData.append('steps', '1');  // Positive = down/clockwise
                
                const response = await fetch('/api/input/encoder', {
                    method: 'POST',
                    body: formData
                });
                
                if (response.ok) {
                    setTimeout(updateStatus, 100);  // Quick refresh
                } else {
                    const error = await response.json();
                    showAlert('statusAlert', error.error || 'Failed to send input', 'error');
                }
            } catch (error) {
                showAlert('statusAlert', 'Failed to communicate with device', 'error');
            }
        }

        async function pressButton() {
            try {
                const response = await fetch('/api/input/button', {
                    method: 'POST'
                });
                
                if (response.ok) {
                    setTimeout(updateStatus, 100);  // Quick refresh
                } else {
                    const error = await response.json();
                    showAlert('statusAlert', error.error || 'Failed to send input', 'error');
                }
            } catch (error) {
                showAlert('statusAlert', 'Failed to communicate with device', 'error');
            }
        }

        // Quick Action functions
        function showScheduleProofing() {
            document.getElementById('scheduleForm').style.display = 'block';
        }

        function hideScheduleProofing() {
            document.getElementById('scheduleForm').style.display = 'none';
        }

        async function startProofingNow() {
            if (!confirm('Start proofing now?')) return;
            
            try {
                const response = await fetch('/api/action/proof-now', {
                    method: 'POST'
                });
                
                if (response.ok) {
                    showAlert('statusAlert', '✓ Proofing started!', 'success');
                    setTimeout(updateStatus, 500);
                } else {
                    const error = await response.json();
                    showAlert('statusAlert', error.error || 'Failed to start proofing', 'error');
                }
            } catch (error) {
                showAlert('statusAlert', 'Failed to communicate with device', 'error');
            }
        }

        async function scheduleProofing() {
            const timeInput = document.getElementById('proofAtTime').value;
            const hoursInput = document.getElementById('proofInHours').value;
            
            if (!timeInput && !hoursInput) {
                showAlert('statusAlert', 'Please enter either a time or duration', 'error');
                return;
            }
            
            try {
                let endpoint, body;
                
                if (timeInput) {
                    // Schedule at specific time
                    const [hours, minutes] = timeInput.split(':').map(Number);
                    endpoint = '/api/action/proof-at';
                    body = JSON.stringify({ hour: hours, minute: minutes });
                } else {
                    // Schedule after delay
                    endpoint = '/api/action/proof-in';
                    body = JSON.stringify({ hours: parseFloat(hoursInput) });
                }
                
                const response = await fetch(endpoint, {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: body
                });
                
                if (response.ok) {
                    showAlert('statusAlert', '✓ Proofing scheduled!', 'success');
                    hideScheduleProofing();
                    setTimeout(updateStatus, 500);
                } else {
                    const error = await response.json();
                    showAlert('statusAlert', error.error || 'Failed to schedule proofing', 'error');
                }
            } catch (error) {
                showAlert('statusAlert', 'Failed to communicate with device', 'error');
            }
        }

        async function stopOperation() {
            if (!confirm('Stop current operation?')) return;
            
            try {
                const response = await fetch('/api/action/stop', {
                    method: 'POST'
                });
                
                if (response.ok) {
                    showAlert('statusAlert', '✓ Operation stopped', 'success');
                    setTimeout(updateStatus, 500);
                } else {
                    const error = await response.json();
                    showAlert('statusAlert', error.error || 'Failed to stop', 'error');
                }
            } catch (error) {
                showAlert('statusAlert', 'Failed to communicate with device', 'error');
            }
        }

        async function saveSettings() {
            try {
                const formData = new FormData();
                formData.append('heating_lower', document.getElementById('heatingLower').value);
                formData.append('heating_upper', document.getElementById('heatingUpper').value);
                formData.append('cooling_lower', document.getElementById('coolingLower').value);
                formData.append('cooling_upper', document.getElementById('coolingUpper').value);
                
                const response = await fetch('/api/settings', {
                    method: 'POST',
                    body: formData
                });
                
                if (response.ok) {
                    showAlert('settingsAlert', 'Settings saved successfully!', 'success');
                } else {
                    const error = await response.json();
                    showAlert('settingsAlert', error.message || 'Failed to save settings', 'error');
                }
            } catch (error) {
                showAlert('settingsAlert', 'Failed to communicate with device', 'error');
            }
        }

        // Initialize WebSocket for real-time updates
        initWebSocket();
        
        // Initial load
        loadSettings();
    </script>
</body>
</html>)html";
}

} // namespace services
