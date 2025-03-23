#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H

#include "ScreensManager.h"
#include "screens/AdjustValue.h"
#include "screens/AdjustTime.h"
#include "screens/ProofingScreen.h"
#include "screens/WiFiReset.h"

class MenuActions {
public:
    MenuActions(ScreensManager* screensManager, AdjustValue* adjustValue, AdjustTime* adjustTime, ProofingScreen* proofingScreen, WiFiReset* wifiReset);
    void proofNowAction();
    void proofInAction();
    void proofAtAction();
    void adjustHotTargetTemp();
    void adjustHotLowerLimit();
    void adjustHotHigherLimit();
    void adjustColdTargetTemp();
    void adjustColdLowerLimit();
    void adjustColdHigherLimit();
    void resetWiFiAndReboot();
    void adjustTimezone();
    ScreensManager* _screensManager;
private:
    AdjustTime* _adjustTime;
    AdjustValue* _adjustValue;
    ProofingScreen* _proofingScreen;
    WiFiReset* _wifiReset;
};

#endif
