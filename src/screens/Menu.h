#pragma once

#include <Arduino.h>
#include <functional>
#include "screens/BaseController.h"
#include "IDisplayManager.h"
#include "IInputManager.h"
#include "ScreensManager.h"
#include "MenuActions.h"
#include "AppContextDecl.h"

class Menu : public BaseController {
public:
    struct MenuItem {
        const char* name;               // Name of the menu item (stored in PROGMEM)
        const uint8_t* icon;            // Icon for the menu item (nullptr if no icon)
        MenuItem* subMenu;              // Pointer to submenu (nullptr if no submenu)
        void (MenuActions::*action)();  // Action as method pointer (nullptr if no action)
    };

    Menu(AppContext* ctx, MenuActions* menuActions);
    void begin();
    void beginImpl() override;
    bool update(bool forceRedraw = false) override;

private:
    MenuItem* _currentMenu;
    uint8_t _menuIndex = 0;

    MenuActions* _menuActions;
    IDisplayManager* _display;

    // Helper functions
    void drawMenu();
    uint8_t getCurrentMenuSize() const;
    bool handleMenuSelection();
};
