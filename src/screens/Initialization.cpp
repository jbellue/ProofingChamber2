#include <WiFiManager.h>
#include <time.h>
#include "DebugUtils.h"
#include "Initialization.h"
#include "icons.h"

Initialization::Initialization(DisplayManager* display) : _display(display)
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

    WiFiManager wifiManager;
    wifiManager.autoConnect();
    _display->drawUTF8(0, 34, "Succ\xC3\xA8s.");
    _display->drawStr(0, 46, "Connexion au NTP...");
    _display->sendBuffer();
    _display->setCursor(0, 58);

    // Configure NTP
    const char* timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Europe/Paris timezone (https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)
    configTzTime(timezone, "pool.ntp.org", "time.nist.gov"); // Configure NTP with default servers
    DEBUG_PRINT("Waiting for NTP time sync");
    while (time(nullptr) < 1000000000) { // Wait until the time is synced
        delay(500);
        DEBUG_PRINT(".");
        _display->print(".");
        _display->sendBuffer();
    }
    DEBUG_PRINTLN("\nTime synced with NTP");
}
