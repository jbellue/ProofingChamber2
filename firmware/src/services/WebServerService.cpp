#include "WebServerService.h"
#include "../AppContext.h"
#include "../ITemperatureController.h"
#include "../IInputManager.h"
#include "../StorageConstants.h"
#include "../services/IStorage.h"
#include "../DebugUtils.h"
#include <ArduinoJson.h>

namespace services {

WebServerService::WebServerService(AppContext* ctx)
    : _ctx(ctx), _server(80) {
}

void WebServerService::begin() {
    setupRoutes();
    _server.begin();
    DEBUG_PRINTLN("Web server started on port 80");
}

void WebServerService::update() {
    // AsyncWebServer handles requests asynchronously, no polling needed
}

void WebServerService::setupRoutes() {
    // Serve the main web page
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetRoot(request);
    });
    
    // API endpoints
    _server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetStatus(request);
    });
    
    _server.on("/api/settings", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetSettings(request);
    });
    
    _server.on("/api/mode", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleSetMode(request);
    });
    
    _server.on("/api/settings", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleSetSettings(request);
    });
    
    _server.on("/api/cooling/schedule", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleScheduleCooling(request);
    });
}

void WebServerService::handleGetRoot(AsyncWebServerRequest* request) {
    request->send(200, "text/html", getWebPageHtml());
}

void WebServerService::handleGetStatus(AsyncWebServerRequest* request) {
    JsonDocument doc;
    
    if (_ctx->input) {
        doc["temperature"] = _ctx->input->getTemperature();
    } else {
        doc["temperature"] = nullptr;
    }
    
    if (_ctx->tempController) {
        ITemperatureController::Mode mode = _ctx->tempController->getMode();
        switch (mode) {
            case ITemperatureController::HEATING:
                doc["mode"] = "heating";
                break;
            case ITemperatureController::COOLING:
                doc["mode"] = "cooling";
                break;
            case ITemperatureController::OFF:
                doc["mode"] = "off";
                break;
        }
        doc["isHeating"] = _ctx->tempController->isHeating();
        doc["isCooling"] = _ctx->tempController->isCooling();
    } else {
        doc["mode"] = "unknown";
        doc["isHeating"] = false;
        doc["isCooling"] = false;
    }
    
    // Add timing information
    struct tm now;
    getLocalTime(&now);
    time_t currentTime = mktime(&now);
    
    if (_ctx->proofingController && _ctx->proofingController->isActive()) {
        time_t startTime = _ctx->proofingController->getStartTime();
        doc["proofingStartTime"] = (long)startTime;
        doc["proofingElapsedSeconds"] = (long)difftime(currentTime, startTime);
    }
    
    if (_ctx->coolingController && _ctx->coolingController->isActive()) {
        time_t endTime = _ctx->coolingController->getEndTime();
        doc["coolingEndTime"] = (long)endTime;
        doc["coolingRemainingSeconds"] = (long)difftime(endTime, currentTime);
    }
    
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
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
    
    if (!_ctx->tempController) {
        request->send(500, "application/json", "{\"error\":\"Temperature controller not available\"}");
        return;
    }
    
    if (mode == "heating") {
        _ctx->tempController->setMode(ITemperatureController::HEATING);
        request->send(200, "application/json", "{\"status\":\"ok\",\"mode\":\"heating\"}");
    } else if (mode == "cooling") {
        _ctx->tempController->setMode(ITemperatureController::COOLING);
        request->send(200, "application/json", "{\"status\":\"ok\",\"mode\":\"cooling\"}");
    } else if (mode == "off") {
        _ctx->tempController->setMode(ITemperatureController::OFF);
        request->send(200, "application/json", "{\"status\":\"ok\",\"mode\":\"off\"}");
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

String WebServerService::getWebPageHtml() {
    return R"html(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Proofing Chamber Control</title>
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
            <h2>Mode Control</h2>
            <div class="mode-buttons">
                <button class="btn btn-heating" onclick="setMode('heating')">
                    🔥 Heating
                </button>
                <button class="btn btn-cooling" onclick="setMode('cooling')">
                    ❄️ Cooling
                </button>
                <button class="btn btn-off" onclick="setMode('off')">
                    ⏸️ Off
                </button>
            </div>
        </div>

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

        async function updateStatus() {
            try {
                const response = await fetch('/api/status');
                const data = await response.json();
                
                document.getElementById('temperature').textContent = 
                    data.temperature !== null ? data.temperature.toFixed(1) + '°C' : '--';
                
                currentMode = data.mode || 'off';
                const modeText = currentMode.charAt(0).toUpperCase() + currentMode.slice(1);
                document.getElementById('modeText').textContent = modeText;
                
                const indicator = document.getElementById('modeIndicator');
                indicator.className = `indicator ${currentMode}`;
                
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
                
                // Update button states
                document.querySelectorAll('.mode-buttons .btn').forEach(btn => {
                    btn.classList.remove('active');
                });
                if (currentMode === 'heating') {
                    document.querySelector('.btn-heating').classList.add('active');
                } else if (currentMode === 'cooling') {
                    document.querySelector('.btn-cooling').classList.add('active');
                } else {
                    document.querySelector('.btn-off').classList.add('active');
                }
            } catch (error) {
                console.error('Failed to update status:', error);
            }
        }

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

        async function setMode(mode) {
            try {
                const formData = new FormData();
                formData.append('mode', mode);
                
                const response = await fetch('/api/mode', {
                    method: 'POST',
                    body: formData
                });
                
                if (response.ok) {
                    showAlert('statusAlert', `Mode changed to ${mode}`, 'success');
                    updateStatus();
                } else {
                    const error = await response.json();
                    showAlert('statusAlert', error.error || 'Failed to change mode', 'error');
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

        // Update status every 2 seconds
        setInterval(updateStatus, 2000);
        
        // Initial load
        updateStatus();
        loadSettings();
    </script>
</body>
</html>)html";
}

} // namespace services
