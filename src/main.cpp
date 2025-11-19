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
#include "services/RebootService.h"
#include "services/NetworkService.h"
#include "services/StorageAdapter.h"
#include "services/IStorage.h"
#include "screens/Reboot.h"
#include "screens/SetTimezone.h"
#include "screens/WiFiReset.h"
#include "ScreensManager.h"
#include "Storage.h"
#include "TemperatureController.h"
#include "AppContextDecl.h"

#define DS18B20_PIN       0
#define COOLING_RELAY_PIN 1
#define HEATING_RELAY_PIN 2
#define ENCODER_CLK       3
#define ENCODER_DT        4
#define ENCODER_SW        10

// Global objects
DisplayManager displayManager(U8G2_R0);
// Storage adapter and temperature controller depend on storage
services::StorageAdapter storageAdapter;
TemperatureController temperatureController(HEATING_RELAY_PIN, COOLING_RELAY_PIN);

ScreensManager screensManager;
InputManager inputManager(ENCODER_CLK, ENCODER_DT, ENCODER_SW, DS18B20_PIN);
// Global AppContext instance (defined early so globals can receive its pointer)
AppContext appContext;

AdjustValue adjustValue(&appContext);
AdjustTime adjustTime(&appContext);
ProofingScreen proofingScreen(&appContext);
SetTimezone setTimezone(&appContext);
// Network and reboot services
services::NetworkService networkService;
services::RebootService rebootService;

// Screens (constructed with AppContext where applicable)
Reboot reboot(&appContext);
Initialization initialization(&appContext);
CoolingScreen coolingScreen(&appContext);
WiFiReset wifiReset(&appContext);

MenuActions menuActions(&appContext, &adjustValue, &adjustTime, &proofingScreen, &coolingScreen, &wifiReset, &setTimezone, &reboot);
Menu menu(&appContext, &menuActions);

void setup() {
#if DEBUG
    // Initialize serial communication
    Serial.begin(115200);
#endif
    if (!storageAdapter.begin()) {
        // Handle initialization failure
        DEBUG_PRINTLN("Storage initialization failed");
    }
    displayManager.begin();
    inputManager.begin();
    temperatureController.begin();

    // Populate global AppContext so components can access shared services
    appContext.display = &displayManager;
    appContext.input = &inputManager;
    appContext.screens = &screensManager;
    appContext.tempController = &temperatureController;
    appContext.rebootService = &rebootService;
        appContext.networkService = &networkService;
        appContext.storage = &storageAdapter;

    // Provide storage to TemperatureController now that AppContext.storage is set
    temperatureController.setStorage(appContext.storage);

    initialization.setNextScreen(&menu);
    screensManager.setActiveScreen(&initialization);
}

void loop() {
    inputManager.update();
    screensManager.update();
}
