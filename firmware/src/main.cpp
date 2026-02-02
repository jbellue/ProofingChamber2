#include <U8g2lib.h>
#include <driver/gpio.h>
#include <esp_log.h>
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
#include "screens/controllers/WiFiResetController.h"
#include "screens/controllers/CoolingController.h"
#include "screens/controllers/DataDisplayController.h"
#include "screens/controllers/ConfirmTimezoneController.h"
#include "screens/controllers/PowerOffController.h"
#include "screens/views/AdjustValueView.h"
#include "screens/views/AdjustTimeView.h"
#include "screens/views/CoolingView.h"
#include "screens/views/ProofingView.h"
#include "screens/views/RebootView.h"
#include "screens/views/WiFiResetView.h"
#include "screens/views/DataDisplayView.h"
#include "screens/views/ConfirmTimezoneView.h"
#include "screens/views/PowerOffView.h"
#include "ScreensManager.h"
#include "Storage.h"
#include "TemperatureController.h"
#include "AppContextDecl.h"

#define DS18B20_PIN       GPIO_NUM_20 //update prototype
#define COOLING_RELAY_PIN GPIO_NUM_2  //update prototype
#define HEATING_RELAY_PIN GPIO_NUM_1  //update prototype
#define ENCODER_CLK       GPIO_NUM_6  //update prototype
#define ENCODER_DT        GPIO_NUM_7  //update prototype
#define ENCODER_SW        GPIO_NUM_5
#define PROOFING_LED_PIN  GPIO_NUM_10 //update prototype
#define COOLING_LED_PIN   GPIO_NUM_21 //update prototype

// Global objects
DisplayManager displayManager(U8G2_R0);
// Storage adapter and temperature controller depend on storage
services::StorageAdapter storageAdapter;
TemperatureController temperatureController(HEATING_RELAY_PIN, COOLING_RELAY_PIN, PROOFING_LED_PIN, COOLING_LED_PIN);

ScreensManager screensManager;
InputManager inputManager(ENCODER_CLK, ENCODER_DT, ENCODER_SW, DS18B20_PIN);
// Global AppContext instance (defined early so globals can receive its pointer)
AppContext appContext;

// Static controller instances (stack allocated)
static AdjustValueController adjustValueControllerInstance(&appContext);
static AdjustTimeController adjustTimeControllerInstance(&appContext);
static ProofingController proofingControllerInstance(&appContext);
static RebootController rebootInstance(&appContext);
static CoolingController coolingControllerInstance(&appContext);
static WiFiResetController wifiResetControllerInstance(&appContext);
static DataDisplayController dataDisplayControllerInstance(&appContext);
static ConfirmTimezoneController confirmTimezoneControllerInstance(&appContext);
static PowerOffController powerOffControllerInstance(&appContext);

AdjustValueController* adjustValueController = &adjustValueControllerInstance;
AdjustTimeController* adjustTimeController = &adjustTimeControllerInstance;
ProofingController* proofingController = &proofingControllerInstance;
RebootController* reboot = &rebootInstance;
CoolingController* coolingController = &coolingControllerInstance;
WiFiResetController* wifiResetController = &wifiResetControllerInstance;
DataDisplayController* dataDisplayController = &dataDisplayControllerInstance;
ConfirmTimezoneController* confirmTimezoneController = &confirmTimezoneControllerInstance;
PowerOffController* powerOffController = &powerOffControllerInstance;
Initialization* initialization = nullptr; // Created in setup after network service

MenuActions* menuActions = nullptr; // Created in setup
Menu* menu = nullptr; // Created in setup

// Network and reboot services
services::NetworkService networkService;
services::RebootService rebootService;

// Static view instances (allocated on stack)
static AdjustValueView adjustValueView(&displayManager);
static AdjustTimeView adjustTimeView(&displayManager);
static CoolingView coolingView(&displayManager);
static ProofingView proofingView(&displayManager);
static RebootView rebootView(&displayManager);
static WiFiResetView wifiResetView(&displayManager);
static DataDisplayView dataDisplayView(&displayManager);
static ConfirmTimezoneView confirmTimezoneView(&displayManager);
static PowerOffView powerOffView(&displayManager);

void setup() {
#if defined CORE_DEBUG_LEVEL && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_NONE
    // Initialize serial communication
    Serial.begin(115200);
#endif
    // Release any deep-sleep GPIO holds from previous power-off
    gpio_deep_sleep_hold_dis();
    gpio_hold_dis(COOLING_LED_PIN);
    gpio_hold_dis(PROOFING_LED_PIN);
    gpio_hold_dis(HEATING_RELAY_PIN);
    gpio_hold_dis(COOLING_RELAY_PIN);
    if (!storageAdapter.begin()) {
        DEBUG_PRINTLN("Storage initialization failed - using safe defaults");
        // Set safe default temperatures (moderate room temperature range)
        temperatureController.setDefaultLimits(23, 27);
    }
    displayManager.begin();
    inputManager.begin();
    temperatureController.begin();

    // Initialize output pin
    gpio_reset_pin(COOLING_LED_PIN);
    gpio_set_direction(COOLING_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_reset_pin(PROOFING_LED_PIN);
    gpio_set_direction(PROOFING_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_reset_pin(HEATING_RELAY_PIN);
    gpio_set_direction(HEATING_RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_reset_pin(COOLING_RELAY_PIN);
    gpio_set_direction(COOLING_RELAY_PIN, GPIO_MODE_OUTPUT);

    // Populate global AppContext so components can access shared services
    appContext.display = &displayManager;
    appContext.input = &inputManager;
    appContext.screens = &screensManager;
    appContext.tempController = &temperatureController;
    appContext.rebootService = &rebootService;
    appContext.networkService = &networkService;
    appContext.storage = &storageAdapter;
    appContext.encoderButtonPin = ENCODER_SW;
    appContext.heaterRelayPin = HEATING_RELAY_PIN;
    appContext.coolerRelayPin = COOLING_RELAY_PIN;
    appContext.proofingLedPin = PROOFING_LED_PIN;
    appContext.coolingLedPin = COOLING_LED_PIN;
    
    // Add view pointers to AppContext
    appContext.adjustValueView = &adjustValueView;
    appContext.adjustTimeView = &adjustTimeView;
    appContext.coolingView = &coolingView;
    appContext.proofingView = &proofingView;
    appContext.rebootView = &rebootView;
    appContext.wifiResetView = &wifiResetView;
    appContext.dataDisplayView = &dataDisplayView;
    appContext.confirmTimezoneView = &confirmTimezoneView;
    appContext.powerOffView = &powerOffView;

    // Provide storage to TemperatureController now that AppContext.storage is set
    temperatureController.setStorage(appContext.storage);

    // Create remaining objects that depend on appContext being fully initialized
    static Initialization initializationInstance(&appContext);
    initialization = &initializationInstance;
    
    static MenuActions menuActionsInstance(&appContext, adjustValueController, adjustTimeController, proofingController, coolingController, wifiResetController, reboot, dataDisplayController, confirmTimezoneController, powerOffController);
    menuActions = &menuActionsInstance;
    
    static Menu menuInstance(&appContext, menuActions);
    menu = &menuInstance;
    
    // Initialize dynamic timezone menus
    initializeAllMenus(&appContext);
    
    // Set the menu instance in MenuActions for context-aware actions
    menuActions->setMenu(&menuInstance);

    initialization->setNextScreen(menu);
    screensManager.setActiveScreen(initialization);
}

void loop() {
    inputManager.update();
    screensManager.update();
}
