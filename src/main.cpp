#include <U8g2lib.h>
#include "DebugUtils.h"
#include "DisplayManager.h"
#include "DS18B20Manager.h"
#include "InputManager.h"
#include "MenuActions.h"
#include "MenuItems.h"
#include "screens/AdjustTime.h"
#include "screens/AdjustValue.h"
#include "screens/CoolingScreen.h"
#include "screens/Initialization.h"
#include "screens/Menu.h"
#include "screens/ProofingScreen.h"
#include "screens/SetTimezone.h"
#include "screens/WiFiReset.h"
#include "ScreensManager.h"
#include "Storage.h"

#define DS18B20_PIN 0
#define ENCODER_CLK 2
#define ENCODER_DT  3
#define ENCODER_SW  10

// Global objects
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
DisplayManager displayManager(&display);

ScreensManager screensManager;
DS18B20Manager ds18b20Manager(DS18B20_PIN);
InputManager inputManager(ENCODER_CLK, ENCODER_DT, ENCODER_SW, &ds18b20Manager);
AdjustValue adjustValue(&displayManager, &inputManager);
AdjustTime adjustTime(&displayManager, &inputManager);
ProofingScreen proofingScreen(&displayManager, &inputManager);
SetTimezone setTimezone(&displayManager, &inputManager);
Initialization initialization(&displayManager);
CoolingScreen coolingScreen(&displayManager, &inputManager);
WiFiReset wifiReset(&displayManager, &inputManager);
MenuActions menuActions(&screensManager, &adjustValue, &adjustTime, &proofingScreen, &coolingScreen, &wifiReset, &setTimezone);
Menu menu(&displayManager, &inputManager, &menuActions);

void setup() {
#if DEBUG
    // Initialize serial communication
    Serial.begin(115200);
#endif
    if (!Storage::begin()) {
        // Handle initialization failure
        DEBUG_PRINTLN("Storage initialization failed");
    }
    ds18b20Manager.begin();
    displayManager.begin();
    inputManager.begin();

    initialization.setNextScreen(&menu);
    screensManager.setActiveScreen(&initialization);
}

void loop() {
    inputManager.update();
    screensManager.update();
}
