#include "Menu.h"
#include <WifiManager.h>
#include <WiFi.h>
#include <time.h>
#include <U8g2lib.h>

// Global objects
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
Menu menu(&display);

void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Initialize the display
    display.begin();
    display.clearBuffer();
    display.setFont(u8g2_font_t0_11_tf);
    display.drawStr(0, 10, "Connecting to Wi-Fi...");
    display.sendBuffer();

    WiFiManager wifiManager;
    wifiManager.autoConnect();
    display.drawStr(0, 20, "done.");
    display.drawStr(0, 30, "Connecting to NTP server...");
    display.sendBuffer();
    display.setCursor(0, 40);

    // Configure NTP
    const char* timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Europe/Paris timezone (https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)
    configTzTime(timezone, "pool.ntp.org", "time.nist.gov"); // Configure NTP with default servers
    Serial.print("Waiting for NTP time sync");
    while (time(nullptr) < 1000000000) { // Wait until the time is synced
        delay(500);
        Serial.print(".");
        display.print(".");
        display.sendBuffer();
    }
    Serial.println("\nTime synced with NTP");

    // Initialize menu
    menu.begin();
}

void loop() {
    menu.update();
}
