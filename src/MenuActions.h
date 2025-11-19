#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H

#include "ScreensManager.h"
#include "screens/AdjustValue.h"
#include "screens/AdjustTime.h"
#include "screens/ProofingScreen.h"
#include "screens/CoolingScreen.h"
#include "screens/WiFiReset.h"
#include "screens/Reboot.h"
#include "screens/SetTimezone.h"
#include "AppContextDecl.h"

class MenuActions {
public:
    MenuActions(AppContext* ctx, AdjustValue* adjustValue, AdjustTime* adjustTime, ProofingScreen* proofingScreen, CoolingScreen* coolingScreen, WiFiReset* wifiReset, SetTimezone* setTimezone, Reboot* reboot);
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
    void reboot();
    void adjustTimezone();
private:
    AppContext* _ctx;
    AdjustTime* _adjustTime;
    AdjustValue* _adjustValue;
    ProofingScreen* _proofingScreen;
    CoolingScreen* _coolingScreen;
    WiFiReset* _wifiReset;
    SetTimezone* _setTimezone;
    Reboot* _reboot;
};

#endif
