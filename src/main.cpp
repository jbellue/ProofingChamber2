#include <U8g2lib.h>
#include "DebugUtils.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "MenuActions.h"
#include "MenuItems.h"
#include "screens/Initialization.h"
#include "screens/Menu.h"
#include "services/RebootService.h"
#include "services/NetworkService.h"
#include "services/StorageAdapter.h"
#include "services/IStorage.h"
#include "screens/controllers/ProofingController.h"
#include "screens/controllers/AdjustTimeController.h"
#include "screens/controllers/AdjustValueController.h"
#include "screens/controllers/RebootController.h"
#include "screens/controllers/SetTimezoneController.h"
#include "screens/controllers/WiFiResetController.h"
#include "screens/controllers/CoolingController.h"
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
// Screen/service pointers — constructed in setup() to avoid static init order issues
AdjustValueController* adjustValueController = nullptr;
AdjustTimeController* adjustTimeController = nullptr;
ProofingController* proofingController = nullptr;
SetTimezoneController* setTimezoneController = nullptr;
// Network and reboot services
services::NetworkService networkService;
services::RebootService rebootService;

// Screen pointers (created in setup)
RebootController* reboot = nullptr;
Initialization* initialization = nullptr;
CoolingController* coolingController = nullptr;
WiFiResetController* wifiResetController = nullptr;

MenuActions* menuActions = nullptr;
Menu* menu = nullptr;

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

    // Now create screens and menu objects — pass `&appContext` so they can bind in beginImpl
    adjustValueController = new AdjustValueController(&appContext);
    adjustTimeController = new AdjustTimeController(&appContext);
    proofingController = new ProofingController(&appContext);
    setTimezoneController = new SetTimezoneController(&appContext);

    reboot = new RebootController(&appContext);
    initialization = new Initialization(&appContext);
    coolingController = new CoolingController(&appContext);
    wifiResetController = new WiFiResetController(&appContext);

    menuActions = new MenuActions(&appContext, adjustValueController, adjustTimeController, proofingController, coolingController, wifiResetController, setTimezoneController, reboot);
    menu = new Menu(&appContext, menuActions);

    initialization->setNextScreen(menu);
    screensManager.setActiveScreen(initialization);
}

void loop() {
    inputManager.update();
    screensManager.update();
}
