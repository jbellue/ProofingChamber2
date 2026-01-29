#include <time.h>
#include <U8g2lib.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "DebugUtils.h"
#include "Initialization.h"
#include "icons.h"
// Need the concrete service definition to call methods like autoConnect()/configureNtp()
#include "../services/INetworkService.h"
// Storage interface to retrieve timezone configuration
#include "../services/IStorage.h"
#include "StorageConstants.h"

Initialization::Initialization(AppContext* ctx) : BaseController(ctx), _display(nullptr), _networkService(nullptr), _storage(nullptr)
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
    }
    _display->clear();
}

bool Initialization::update(bool forceRedraw) {
    drawScreen();
    return false; // No need to update - move to the next screen immediately
}


void Initialization::drawScreen() {
    _display->clearBuffer();
    _display->setFont(u8g2_font_t0_11_tf);
    _display->drawStr(0, 10, "Initialisation...");
    _display->drawStr(0, 22, "Connexion au WiFi...");
    _display->sendBuffer();

    // Use the injected network service to connect; continue on failure after a short timeout
    bool wifiConnected = false;
    // Name the captive portal so users can spot it easily
    const char* portalName = "ProofingChamber";
    wifiConnected = _networkService->autoConnect(portalName, [this](const char* apName) {
        if (!_display) return;
        _display->clearBuffer();
        _display->setFont(u8g2_font_t0_11_tf);
        _display->drawStr(0, 10, "Portail WiFi actif");
        _display->drawStr(0, 22, "Connectez-vous :");
        const char* safeName = apName ? apName : "ConfigPortal";
        _display->drawStr(0, 34, safeName);
        _display->sendBuffer();
    });
    if (!wifiConnected) {
        _display->drawUTF8(0, 34, "WiFi indisponible.");
        _display->sendBuffer();
        return; // Do not hang the boot if WiFi is down
    }

    _display->drawUTF8(0, 34, "Succ\xC3\xA8s.");
    _display->drawStr(0, 46, "Connexion au NTP...");
    _display->sendBuffer();
    _display->setCursor(0, 58);

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
        _display->print(".");
        _display->sendBuffer();
        taskYIELD();
    }
    DEBUG_PRINTLN("\nTime synced with NTP");
}
