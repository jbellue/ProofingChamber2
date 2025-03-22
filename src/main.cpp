#include <WifiManager.h>
#include <WiFi.h>
#include <time.h>
#include <U8g2lib.h>
#include "DisplayManager.h"
#include "DebugUtils.h"
#include "InputManager.h"
#include "Screens/Menu.h"
#include "MenuActions.h"
#include "MenuItems.h"
#include "ScreensManager.h"
#include "screens/AdjustTime.h"
#include "screens/AdjustValue.h"
#include "screens/ProofingScreen.h"

#define ENCODER_CLK 2
#define ENCODER_DT  3
#define ENCODER_SW  4

// Global objects
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
DisplayManager displayManager(&display);

ScreensManager screensManager;
InputManager inputManager(ENCODER_CLK, ENCODER_DT, ENCODER_SW);
AdjustValue adjustValue(&displayManager, &inputManager);
AdjustTime adjustTime(&displayManager, &inputManager);
ProofingScreen proofingScreen(&displayManager, &inputManager);
MenuActions menuActions(&screensManager, &adjustValue, &adjustTime, &proofingScreen);
Menu menu(&displayManager, &inputManager, &menuActions);

void setup() {
#if DEBUG
    // Initialize serial communication
    Serial.begin(115200);
#endif

    // Initialize the display
    display.begin();
    display.clearBuffer();
    display.setFont(u8g2_font_t0_11_tf);
    display.drawStr(0, 10, "Initialization...");
    display.drawStr(0, 22, "Connecting to WiFi...");
    display.sendBuffer();

    WiFiManager wifiManager;
    wifiManager.autoConnect();
    display.drawStr(0, 34, "done.");
    display.drawStr(0, 46, "Connecting to NTP...");
    display.sendBuffer();
    display.setCursor(0, 58);

    // Configure NTP
    const char* timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Europe/Paris timezone (https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)
    configTzTime(timezone, "pool.ntp.org", "time.nist.gov"); // Configure NTP with default servers
    DEBUG_PRINT("Waiting for NTP time sync");
    while (time(nullptr) < 1000000000) { // Wait until the time is synced
        delay(500);
        DEBUG_PRINT(".");
        display.print(".");
        display.sendBuffer();
    }
    DEBUG_PRINTLN("\nTime synced with NTP");

    // Initialize menu
    menu.begin(mainMenu);
    screensManager.setActiveScreen(&menu);
}

void loop() {
    inputManager.update();
    screensManager.update();
}
