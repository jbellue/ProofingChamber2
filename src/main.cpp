#include <U8g2lib.h>
#include "DebugUtils.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "MenuActions.h"
#include "MenuItems.h"
#include "screens/AdjustTime.h"
#include "screens/AdjustValue.h"
#include "screens/CoolingScreen.h"
#include "screens/Initialization.h"
#include "screens/Menu.h"
#include "screens/ProofingScreen.h"
#include "screens/Reboot.h"
#include "screens/SetTimezone.h"
#include "screens/WiFiReset.h"
#include "ScreensManager.h"
#include "Storage.h"
#include "TemperatureController.h"

#define DS18B20_PIN       0
#define COOLING_RELAY_PIN 1
#define HEATING_RELAY_PIN 2
#define ENCODER_CLK       3
#define ENCODER_DT        4
#define ENCODER_SW        10

// Global objects
DisplayManager displayManager(U8G2_R0);
TemperatureController temperatureController(HEATING_RELAY_PIN, COOLING_RELAY_PIN);

ScreensManager screensManager;
InputManager inputManager(ENCODER_CLK, ENCODER_DT, ENCODER_SW, DS18B20_PIN);
AdjustValue adjustValue(&displayManager, &inputManager);
AdjustTime adjustTime(&displayManager, &inputManager);
ProofingScreen proofingScreen(&displayManager, &inputManager, &temperatureController);
SetTimezone setTimezone(&displayManager, &inputManager);
Initialization initialization(&displayManager);
CoolingScreen coolingScreen(&displayManager, &inputManager, &temperatureController);
WiFiReset wifiReset(&displayManager, &inputManager);
Reboot reboot(&displayManager, &inputManager);
MenuActions menuActions(&screensManager, &adjustValue, &adjustTime, &proofingScreen, &coolingScreen, &wifiReset, &setTimezone, &reboot);
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
    displayManager.begin();
    inputManager.begin();
    temperatureController.begin();

    initialization.setNextScreen(&menu);
    screensManager.setActiveScreen(&initialization);
}

void loop() {
    inputManager.update();
    screensManager.update();
}
