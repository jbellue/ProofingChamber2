#include <time.h>
#include <U8g2lib.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include "DebugUtils.h"
#include "Initialization.h"
#include "icons.h"
#include "../services/INetworkService.h"
#include "../services/IWebServerService.h"
#include "../services/IStorage.h"
#include "StorageConstants.h"

Initialization::Initialization(AppContext* ctx) : BaseController(ctx), _display(nullptr), _networkService(nullptr), _storage(nullptr), _webServerService(nullptr)
{}

void Initialization::begin() {
    beginImpl();
}

void Initialization::beginImpl() {
    AppContext* ctx = getContext();
    if (ctx) {
        if (!_display) _display = ctx->display;
        if (!_networkService) _networkService = ctx->networkService;
        if (!_storage) _storage = ctx->storage;
        if (!_webServerService) _webServerService = ctx->webServerService;
    }
    _display->clear();
    _buffer.clear();
}

bool Initialization::update(bool forceRedraw) {
    drawScreen();
    return false; // No need to update - move to the next screen immediately
}


void Initialization::drawScreen() {
    _display->clearBuffer();
    _display->setFont(u8g2_font_t0_11_tf);
    _buffer.pushLine("Connexion au WiFi...");
    renderBuffer();

    // Use the injected network service to connect; continue on failure after a short timeout
    bool wifiConnected = false;
    // Name the captive portal so users can spot it easily
    const char* portalName = "Proofi";
    wifiConnected = _networkService->autoConnect(portalName, [this](const char* apName) {
        if (!_display) return;
        _display->setFont(u8g2_font_t0_11_tf);
        _buffer.pushLine("Portail WiFi actif");
        _buffer.pushLine(String(apName));
        renderBuffer();
    });
    if (!wifiConnected) {
        _buffer.pushLine("WiFi indisponible.");
        renderBuffer();
        return; // Do not hang the boot if WiFi is down
    }

        // Start the web server after WiFi is connected
    if (_webServerService) {
        DEBUG_PRINTLN("Starting web server...");
        _webServerService->begin();
        DEBUG_PRINTLN("Web server started successfully");
        _buffer.pushLine("Serveur web actif");
        
        // Get and display IP address
        String ipAddress = WiFi.localIP().toString();
        _buffer.pushLine(ipAddress);
        
        // Also show mDNS hostname
        _buffer.pushLine("proofi.local");
        renderBuffer();
        
        DEBUG_PRINT("Web interface available at: http://");
        DEBUG_PRINTLN(ipAddress.c_str());
        DEBUG_PRINTLN("Also accessible via: http://proofi.local");
    } else {
        if (!wifiConnected) {
            DEBUG_PRINTLN("Web server not started - no WiFi connection");
        } else {
            DEBUG_PRINTLN("Web server not started - service not available");
        }
    }

    _buffer.pushLine("Connexion NTP");
    renderBuffer();

    // Configure NTP via network service
    char timezoneBuf[64];
    _storage->getCharArray(storage::keys::TIMEZONE_KEY, timezoneBuf, sizeof(timezoneBuf), storage::defaults::TIMEZONE_DEFAULT);
    _networkService->configureNtp(timezoneBuf, "pool.ntp.org", "time.nist.gov");
    DEBUG_PRINT("Waiting for NTP time sync");
    const uint64_t timeout_us = esp_timer_get_time() + 30000000ULL; // 30 sec timeout
    while (!_networkService->isTimeSyncReady(1000000000)) {
        if (esp_timer_get_time() > timeout_us) {
            DEBUG_PRINTLN("NTP timeout - continuing anyway");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
        _buffer.appendToLastLine(".");
        renderBuffer();
        taskYIELD();
    }
    DEBUG_PRINTLN("\nTime synced with NTP");
    _buffer.pushLine("Horloge synchronisée");
    renderBuffer();
}

void Initialization::renderBuffer() {
    if (!_display) return;
    _display->clearBuffer();
    _display->setFont(u8g2_font_t0_11_tf);
    for (uint8_t i = 0; i < _buffer.count(); ++i) {
        uint8_t y = (uint8_t)(_baseY + _lineSpacing * i);
        _display->drawUTF8(0, y, _buffer.lineAt(i).c_str());
    }
    _display->sendBuffer();
}
