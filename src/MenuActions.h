#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H

#include "Menu.h"

class MenuActions {
public:
    MenuActions(Menu* menu) {
        _menu = menu;
    }
    static void proofNowAction();
    static void proofInAction();
    static void proofAtAction();
    static void clockAction();
    static void adjustHotTargetTemp();
    static void adjustHotLowerLimit();
    static void adjustHotHigherLimit();
    static void adjustColdTargetTemp();
    static void adjustColdLowerLimit();
    static void adjustColdHigherLimit();
    static void resetWiFiAndReboot();
    static void adjustTimezone();
private:
    static Menu* _menu;
};

#endif
