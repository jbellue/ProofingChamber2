#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H

#include "ScreensManager.h"
#include "screens/controllers/AdjustValueController.h"
#include "screens/controllers/AdjustTimeController.h"
#include "screens/controllers/ProofingController.h"
#include "screens/controllers/CoolingController.h"
#include "screens/controllers/WiFiResetController.h"
#include "screens/controllers/RebootController.h"
#include "screens/controllers/SetTimezoneController.h"
#include "screens/controllers/DataDisplayController.h"
#include "AppContextDecl.h"

class MenuActions {
public:
    MenuActions(AppContext* ctx, AdjustValueController* adjustValueController, AdjustTimeController* adjustTimeController, ProofingController* proofingController, CoolingController* coolingController, WiFiResetController* wifiResetController, SetTimezoneController* setTimezoneController, RebootController* reboot, DataDisplayController* dataDisplayController);
    void proofNowAction();
    void proofInAction();
    void proofAtAction();
    
    // Static callback functions for time calculations
    static time_t calculateProofInEndTime();
    static time_t calculateProofAtEndTime();
    void adjustHotLowerLimit();
    void adjustHotHigherLimit();
    void adjustColdLowerLimit();
    void adjustColdHigherLimit();
    void resetWiFiAndReboot();
    void reboot();
    void adjustTimezone();
    void showDataDisplay();
    
    // Static variables for callback context
    static SimpleTime s_proofInTime;
    static SimpleTime s_proofAtTime;
    
private:
    AppContext* _ctx;
    AdjustTimeController* _adjustTimeController;
    AdjustValueController* _adjustValueController;
    ProofingController* _proofingController;
    CoolingController* _coolingController;
    WiFiResetController* _wifiResetController;
    SetTimezoneController* _setTimezoneController;
    RebootController* _rebootController;
    DataDisplayController* _dataDisplayController;
};

#endif
