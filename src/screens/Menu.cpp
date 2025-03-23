#include <WiFiManager.h>
#include "DebugUtils.h"
#include "Menu.h"
#include "MenuItems.h"
#include "screens/ProofingScreen.h"
#include "icons.h"
#include "screens/Screen.h"

// Constructor
Menu::Menu(DisplayManager* display, InputManager* inputManager, MenuActions* menuActions) :
    _menuActions(menuActions),
    _display(display),
    inputManager(inputManager),
    _currentMenu(nullptr),
    _menuIndex(0)
{}

void Menu::begin() {
    DEBUG_PRINTLN("Menu::begin called");
    beginImpl();
}

// Initialize the menu
void Menu::beginImpl() {
    DEBUG_PRINTLN("Beginning Menu");
    if (_currentMenu == nullptr) {
        _currentMenu = mainMenu;
        _menuIndex = 0;
    }
    _oldPosition = inputManager->getEncoderPosition();
    _display->clear();
}

// Update the menu
bool Menu::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    // Handle encoder rotation
    const int64_t newPosition = inputManager->getEncoderPosition();
    if (newPosition != _oldPosition) {
        if (newPosition > _oldPosition) {
            _menuIndex = (_menuIndex + 1) % getMenuSize(_currentMenu);
        } else {
            _menuIndex = (_menuIndex - 1 + getMenuSize(_currentMenu)) % getMenuSize(_currentMenu);
        }
        _oldPosition = newPosition;
        redraw = true;
    }
    if (redraw) {
        drawMenu(_currentMenu, _menuIndex);
    }

    // Handle encoder button press
    if (inputManager->isButtonPressed()) {
        return handleMenuSelection();
    }
    return true;
}


// Helper functions
void Menu::drawMenu(MenuItem* menu, const uint8_t index) {
    _display->clearBuffer();
    _display->setFontMode(1);
    _display->setBitmapMode(1);
    _display->setFont(u8g2_font_t0_11_tf); // Use a font that supports UTF-8
    for (uint8_t i = 0; menu[i].name != nullptr; i++) {
        const uint8_t yPos = (i + 1) * 16 - 3;
        _display->drawUTF8(16, yPos, menu[i].name);
        if (menu[i].icon != nullptr) {
            _display->drawXBMP(3, yPos - 9, 10, 10, menu[i].icon);
        }
        if (i == index) {
            _display->setDrawColor(2);
            _display->drawRBox(0, yPos - 12, 128, 15, 1);
        }
    }
    _display->sendBuffer();
    DEBUG_PRINTLN("Menu drawn");
}

uint8_t Menu::getMenuSize(MenuItem* menu) {
    uint8_t size = 0;
    while (menu[size].name != nullptr) size++;
    return size;
}

bool Menu::handleMenuSelection() {
    MenuItem* selectedItem = &_currentMenu[_menuIndex];
    if (selectedItem->subMenu != nullptr) {
        _currentMenu = selectedItem->subMenu;
        _menuIndex = 0;
        drawMenu(_currentMenu, _menuIndex);
        DEBUG_PRINTLN("Submenu selected");
    } else if (selectedItem->action != nullptr) {
        selectedItem->action();
        DEBUG_PRINTLN("Action executed");
        return false;
    }
    return true;
}
