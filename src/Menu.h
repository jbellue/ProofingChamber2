#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "Storage.h"
#include "InputManager.h"

class Menu {
public:
    enum MenuState {
        STATE_IDLE,
        STATE_ADJUST_VALUE,
        STATE_ADJUST_TIME
    };

    struct MenuItem {
        const char* name;      // Name of the menu item (stored in PROGMEM)
        const uint8_t* icon;   // Icon for the menu item (nullptr if no icon)
        MenuItem* subMenu;     // Pointer to submenu (nullptr if no submenu)
        void (*action)();      // Pointer to function for action
    };

    Menu(U8G2_SH1106_128X64_NONAME_F_HW_I2C* display, InputManager* inputManager);
    void begin(MenuItem* mainMenu);
    void update();
    uint8_t drawTitle(const uint8_t startY = 10);
    void setCurrentTitle(const char* title);

    void startAdjustValue(const char* title, const char* path);
    void startSetTime(const char* title, const uint8_t startH = 0, const uint8_t startM = 0);
    U8G2_SH1106_128X64_NONAME_F_HW_I2C* display;
    InputManager* inputManager;

private:
    MenuState _currentState;
    Storage _storage;
    uint8_t _encoderSWPin;

    MenuItem* _currentMenu;
    uint8_t _menuIndex = 0;
    int64_t _oldPosition = 0;

    // State variables
    const char* _currentTitle = nullptr;
    const char* _currentPath = nullptr;
    int _currentValue = 0;
    uint8_t _currentDays = 0;
    int8_t _currentHours = 0;
    int8_t _currentMinutes = 0;
    uint8_t _valueY = 0;
    bool _adjustingHours = true;

    // Menu actions
    void updateAdjustValueDisplay();
    void updateAdjustTimeDisplay();

    // State handlers
    void handleMenuNavigation();
    void handleAdjustValue();
    void handleAdjustTime();

    // Helper functions
    void drawMenu(MenuItem* menu, const uint8_t index);
    uint8_t getMenuSize(MenuItem* menu);
    void handleMenuSelection();
};

#endif