#include "Menu.h"
#include <WifiManager.h>
#include <WiFi.h>
#include <time.h>

// Global objects
Menu menu;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    WiFiManager wifiManager;
    wifiManager.autoConnect();

    // Configure NTP
    const char* timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Europe/Paris timezone (https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)
    configTzTime(timezone, "pool.ntp.org", "time.nist.gov"); // Configure NTP with default servers
    Serial.print("Waiting for NTP time sync");
    while (time(nullptr) < 1000000000) { // Wait until the time is synced
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nTime synced with NTP");

    // Initialize menu
    menu.begin();
}

void loop() {
    menu.update();
}
