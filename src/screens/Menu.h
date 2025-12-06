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
    uint8_t _scrollOffset = 0;
    uint8_t _currentMenuSize = 0;

    static const uint8_t MAX_VISIBLE_ITEMS = 4;
    static const uint8_t MENU_ITEM_HEIGHT = 16;
    static const int8_t MENU_ITEM_Y_OFFSET = -3;
    static const uint8_t MENU_ICON_X_OFFSET = 3;
    static const int8_t MENU_ICON_Y_OFFSET = -9;
    static const uint8_t MENU_ICON_WIDTH = 10;
    static const uint8_t MENU_ICON_HEIGHT = 10;
    static const uint8_t MENU_TEXT_X_OFFSET = 16;
    static const uint8_t MENU_SELECTION_X_OFFSET = 0;
    static const int8_t MENU_SELECTION_Y_OFFSET = -12;
    static const uint8_t MENU_SELECTION_HEIGHT = 15;
    static const uint8_t MENU_SELECTION_RADIUS = 1;

    MenuActions* _menuActions;
    IDisplayManager* _display;

    // Helper functions
    void drawMenu();
    uint8_t getCurrentMenuSize() const;
    bool handleMenuSelection();
    void updateScrollOffset();

    void drawNavigationHints(const uint8_t visibleEnd);
};
