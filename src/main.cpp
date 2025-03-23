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
#include "screens/Initialization.h"
#include "screens/ProofingScreen.h"
#include "screens/WiFiReset.h"
#include "screens/SetTimezone.h"

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
SetTimezone setTimezone(&displayManager, &inputManager);
Initialization initialization(&displayManager);
WiFiReset wifiReset(&displayManager, &inputManager);
MenuActions menuActions(&screensManager, &adjustValue, &adjustTime, &proofingScreen, &wifiReset, &setTimezone);
Menu menu(&displayManager, &inputManager, &menuActions);

void setup() {
#if DEBUG
    // Initialize serial communication
    Serial.begin(115200);
#endif

    displayManager.begin();
    inputManager.begin();

    initialization.setNextScreen(&menu);
    screensManager.setActiveScreen(&initialization);
}

void loop() {
    inputManager.update();
    screensManager.update();
}
