#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H

#include "ScreensManager.h"
#include "screens/controllers/AdjustValueController.h"
#include "screens/controllers/AdjustTimeController.h"
#include "screens/controllers/ProofingController.h"
#include "screens/controllers/CoolingController.h"
#include "screens/controllers/WiFiResetController.h"
#include "screens/controllers/RebootController.h"
#include "screens/controllers/DataDisplayController.h"
#include "screens/controllers/ConfirmTimezoneController.h"
#include "screens/controllers/PowerOffController.h"
#include "AppContextDecl.h"

class Menu;  // Forward declaration

class MenuActions {
public:
    MenuActions(AppContext* ctx, AdjustValueController* adjustValueController, AdjustTimeController* adjustTimeController, ProofingController* proofingController, CoolingController* coolingController, WiFiResetController* wifiResetController, RebootController* reboot, DataDisplayController* dataDisplayController, ConfirmTimezoneController* confirmTimezoneController, PowerOffController* powerOffController);
    
    // Set the Menu instance for context-aware actions
    void setMenu(Menu* menu) { _menu = menu; }
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
    void powerOff();
    void showDataDisplay();
    
    // Generic timezone selection handler - uses Menu context to determine selection
    void selectTimezoneByData();
    
    // Static variables for callback context
    static SimpleTime s_proofInTime;
    static SimpleTime s_proofAtTime;
    
private:
    AppContext* _ctx;
    Menu* _menu = nullptr;
    AdjustTimeController* _adjustTimeController;
    AdjustValueController* _adjustValueController;
    ProofingController* _proofingController;
    CoolingController* _coolingController;
    WiFiResetController* _wifiResetController;
    RebootController* _rebootController;
    PowerOffController* _powerOffController;
    DataDisplayController* _dataDisplayController;
    ConfirmTimezoneController* _confirmTimezoneController;
    
    // Helper function to save timezone
    void saveTimezone(const char* posixString);
};

#endif
