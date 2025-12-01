#include "DebugUtils.h"
#include "Menu.h"
#include "MenuItems.h"
#include "screens/controllers/ProofingController.h"
#include "icons.h"
#include "screens/Screen.h"

// Constructor
Menu::Menu(AppContext* ctx, MenuActions* menuActions) :
    _menuActions(menuActions),
    _display(nullptr),
    _inputManager(nullptr),
    _currentMenu(nullptr),
    _menuIndex(0),
    _ctx(ctx)
{}

void Menu::begin() {
    beginImpl();
}

// Initialize the menu
void Menu::beginImpl() {
    DEBUG_PRINTLN("Beginning Menu");
    if (_currentMenu == nullptr) {
        _currentMenu = mainMenu;
        _menuIndex = 0;
    }
    // Late-bind context pointers
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_display) _display = _ctx->display;
    }
    if (_inputManager) _inputManager->resetEncoderPosition();
    if (_display) _display->clear();
}

// Update the menu
bool Menu::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    // Handle encoder rotation
    const auto encoderDirection = _inputManager ? _inputManager->getEncoderDirection() : IInputManager::EncoderDirection::None;
    if (encoderDirection != IInputManager::EncoderDirection::None) {
        if (encoderDirection == IInputManager::EncoderDirection::Clockwise) {
            _menuIndex = (_menuIndex + 1) % getCurrentMenuSize();
        } else {
            _menuIndex = (_menuIndex - 1 + getCurrentMenuSize()) % getCurrentMenuSize();
        }
        redraw = true;
    }
    if (redraw) {
        drawMenu();
    }

    // Handle encoder button press
    if (_inputManager && _inputManager->isButtonPressed()) {
        return handleMenuSelection();
    }
    return true;
}


// Helper functions
void Menu::drawMenu() {
    if (!_display) return;
    _display->clearBuffer();
    _display->setFontMode(1);
    _display->setDrawColor(1);
    _display->setBitmapMode(1);
    _display->setFont(u8g2_font_t0_11_tf); // Use a font that supports UTF-8
    for (uint8_t i = 0; _currentMenu[i].name != nullptr; i++) {
        const uint8_t yPos = (i + 1) * 16 - 3;
        _display->drawUTF8(16, yPos, _currentMenu[i].name);
        if (_currentMenu[i].icon != nullptr) {
            _display->drawXBMP(3, yPos - 9, 10, 10, _currentMenu[i].icon);
        }
        if (i == _menuIndex) {
            _display->setDrawColor(2);
            _display->drawRBox(0, yPos - 12, 128, 15, 1);
        }
    }
    _display->sendBuffer();
}

uint8_t Menu::getCurrentMenuSize() const {
    uint8_t size = 0;
    while (_currentMenu[size].name != nullptr) size++;
    return size;
}

bool Menu::handleMenuSelection() {
    MenuItem* selectedItem = &_currentMenu[_menuIndex];
    if (selectedItem->subMenu != nullptr) {
        _currentMenu = selectedItem->subMenu;
        _menuIndex = 0;
        drawMenu();
        DEBUG_PRINTLN("Submenu selected");
    } else if (selectedItem->action != nullptr && _menuActions != nullptr) {
        (_menuActions->*(selectedItem->action))();
        DEBUG_PRINTLN("Action executed");
        return false;
    }
    return true;
}
