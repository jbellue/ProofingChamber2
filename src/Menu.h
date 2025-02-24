#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <ESP32Encoder.h>
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

    struct MenuItem {
        MenuItemID id;         // Unique identifier for the menu item
        const char* name;      // Name of the menu item (stored in PROGMEM)
        MenuItem* subMenu;     // Pointer to submenu (nullptr if no submenu)
        void (Menu::*action)();// Pointer to member function for action
    };

    Menu(Storage& storage, ESP32Encoder& encoder, int encoderSWPin);
    void begin();
    void update();

private:
    U8G2_SSD1309_128X64_NONAME0_F_HW_I2C _display;
    Storage& _storage;
    ESP32Encoder& _encoder;
    uint8_t _encoderSWPin;

    MenuItem* _currentMenu;
    uint8_t _menuIndex;
    int64_t _oldPosition;
    int _lastButtonState;
    int _buttonState;
    unsigned long _lastDebounceTime;

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
    void adjustValue(const char* title, const char* path);
    void adjustHotTargetTemp();
    void adjustHotLowerLimit();
    void adjustHotHigherLimit();
    void adjustColdTargetTemp();
    void adjustColdLowerLimit();
    void adjustColdHigherLimit();

    // Helper functions
    void drawMenu(MenuItem* menu, int index);
    uint8_t getMenuSize(MenuItem* menu);
    void handleMenuSelection();
};

#endif