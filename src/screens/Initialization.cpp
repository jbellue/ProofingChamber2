#include <time.h>
#include "DebugUtils.h"
#include "Initialization.h"
#include "icons.h"
// Need the concrete service definition to call methods like autoConnect()/configureNtp()
#include "../services/INetworkService.h"

Initialization::Initialization(AppContext* ctx) : _display(ctx->display), _networkService(ctx->networkService), _ctx(ctx)
{}

void Initialization::begin() {
    beginImpl();
}

void Initialization::beginImpl() {
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

    // Use the injected network service to connect
    if (_networkService) {
        _networkService->autoConnect();
    }
    _display->drawUTF8(0, 34, "Succ\xC3\xA8s.");
    _display->drawStr(0, 46, "Connexion au NTP...");
    _display->sendBuffer();
    _display->setCursor(0, 58);

    // Configure NTP via network service
    const char* timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Europe/Paris timezone
    if (_networkService) {
        _networkService->configureNtp(timezone, "pool.ntp.org", "time.nist.gov");
    }
    DEBUG_PRINT("Waiting for NTP time sync");
    while (!_networkService || !_networkService->isTimeSyncReady(1000000000)) { // Wait until the time is synced
        delay(500);
        DEBUG_PRINT(".");
        _display->print(".");
        _display->sendBuffer();
    }
    DEBUG_PRINTLN("\nTime synced with NTP");
}
