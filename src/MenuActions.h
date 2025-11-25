#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H

#include "ScreensManager.h"
#include "screens/AdjustValue.h"
#include "screens/controllers/AdjustTimeController.h"
#include "screens/controllers/ProofingController.h"
#include "screens/controllers/CoolingController.h"
#include "screens/controllers/WiFiResetController.h"
#include "screens/controllers/RebootController.h"
#include "screens/controllers/SetTimezoneController.h"
#include "AppContextDecl.h"

class MenuActions {
public:
    MenuActions(AppContext* ctx, AdjustValue* adjustValue, AdjustTimeController* adjustTimeController, ProofingController* ProofingController, CoolingController* coolingController, WiFiResetController* wifiResetController, SetTimezoneController* setTimezoneController, RebootController* reboot);
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
    AdjustTimeController* _adjustTimeController;
    AdjustValue* _adjustValue;
    ProofingController* _proofingController;
    CoolingController* _coolingController;
    WiFiResetController* _wifiResetController;
    SetTimezoneController* _setTimezoneController;
    RebootController* _rebootController;
};

#endif
