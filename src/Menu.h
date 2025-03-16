#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <U8g2lib.h>
#include "Storage.h"

class Menu {
public:
    // Enum for menu item IDs
    enum MenuItemID {
        MENU_PROOF_NOW,
        MENU_COOL_NOW,
        MENU_SETTINGS,
        MENU_PROOF_IN,
        MENU_PROOF_AT,
        MENU_HOT,
        MENU_COLD,
        MENU_CLOCK,
        MENU_BACK,
        MENU_TARGET_TEMP,
        MENU_LOWER_LIMIT,
        MENU_HIGHER_LIMIT,
        MENU_LAST_ITEM
    };
    enum MenuState {
        STATE_IDLE,
        STATE_ADJUST_VALUE,
        STATE_ADJUST_TIME
    };

    struct MenuItem {
        MenuItemID id;         // Unique identifier for the menu item
        const char* name;      // Name of the menu item (stored in PROGMEM)
        const uint8_t* icon;   // Icon for the menu item (nullptr if no icon)
        MenuItem* subMenu;     // Pointer to submenu (nullptr if no submenu)
        void (Menu::*action)();// Pointer to member function for action
    };

    Menu();
    void begin();
    void update();

private:
    MenuState _currentState;
    U8G2_SH1106_128X64_NONAME_F_HW_I2C _display;
    Storage _storage;
    RotaryEncoder _encoder;
    uint8_t _encoderSWPin;

    MenuItem* _currentMenu;
    uint8_t _menuIndex;
    int64_t _oldPosition;
    int _lastButtonState;
    int _buttonState;
    unsigned long _lastDebounceTime = 0;

    int64_t _encoderPosition = 0; // Store the latest encoder position
    bool _buttonPressed = false;  // Store the latest button state

    // State variables
    const char* _currentTitle = nullptr;
    const char* _currentPath = nullptr;
    int _currentValue = 0;
    uint8_t _currentHours = 0;
    uint8_t _currentMinutes = 0;
    bool _adjustingHours = true;

    void initializeDisplay();

    // Method to handle button press with debounce
    bool isButtonPressed();

    // Menu items
    static MenuItem mainMenu[];
    static MenuItem coolMenu[];
    static MenuItem settingsMenu[];
    static MenuItem hotMenu[];
    static MenuItem coldMenu[];

    // Menu actions
    void proofNowAction();
    void proofInAction();
    void proofAtAction();
    void clockAction();
    void updateAdjustValueDisplay(const char * title, int value);
    void updateAdjustTimeDisplay(const char * title, const uint8_t hours, const uint8_t minutes, bool adjustingHours);
    uint8_t drawTitle(const char * title, const uint8_t startY = 10);
    void adjustHotTargetTemp();
    void adjustHotLowerLimit();
    void adjustHotHigherLimit();
    void adjustColdTargetTemp();
    void adjustColdLowerLimit();
    void adjustColdHigherLimit();


    // State handlers
    void handleMenuNavigation();
    void handleAdjustValue();
    void handleAdjustTime();
    void readInputs();

    void startAdjustValue(const char * title, const char * path);
    void startSetTime(const char * title, const uint8_t startH = 0, const uint8_t startM = 0);

    // Helper functions
    void drawMenu(MenuItem* menu, int index);
    uint8_t getMenuSize(MenuItem* menu);
    void handleMenuSelection();
};

#endif