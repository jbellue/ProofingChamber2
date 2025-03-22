#pragma once

#include <Arduino.h>
#include <functional>
#include "screens/Screen.h"
#include "Storage.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "ScreensManager.h"
#include "MenuActions.h"

class Menu : public Screen {
public:
    struct MenuItem {
        const char* name;               // Name of the menu item (stored in PROGMEM)
        const uint8_t* icon;            // Icon for the menu item (nullptr if no icon)
        MenuItem* subMenu;              // Pointer to submenu (nullptr if no submenu)
        std::function<void()> action;   // Action as a std::function
    };

    Menu(DisplayManager* display, InputManager* inputManager, MenuActions* menuActions);
    void begin(MenuItem* mainMenu);
    void beginImpl() override {}; // Empty implementation for the base class
    bool update(bool forceRedraw = false) override;

private:
    void beginImpl(MenuItem* mainMenu);
    MenuItem* _currentMenu;
    uint8_t _menuIndex = 0;
    int64_t _oldPosition = 0;

    MenuActions* _menuActions;
    DisplayManager* _display;
    InputManager* inputManager;

    // Helper functions
    void drawMenu(MenuItem* menu, const uint8_t index);
    uint8_t getMenuSize(MenuItem* menu);
    void handleMenuSelection();
};
