#include "DebugUtils.h"
#include "Menu.h"
#include "MenuItems.h"
#include "SafePtr.h"
#include "screens/controllers/ProofingController.h"
#include "icons.h"
#include "screens/BaseController.h"

// Constructor
Menu::Menu(AppContext* ctx, MenuActions* menuActions) :
    BaseController(ctx),
    _menuActions(menuActions),
    _display(nullptr),
    _currentMenu(nullptr),
    _menuIndex(0),
    _scrollOffset(0),
    _selectionYPos(0)
{}

void Menu::begin() {
    beginImpl();
}

// Initialize the menu
void Menu::beginImpl() {
    DEBUG_PRINTLN("Beginning Menu");
    // Dynamic menus (like timezones) are initialized in main.cpp with proper AppContext
    if (_currentMenu == nullptr) {
        _currentMenu = mainMenu;
        _menuIndex = 0;
        _scrollOffset = 0;
        _currentMenuSize = getCurrentMenuSize();
        _selectionYPos = MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET;
    }
    initializeInputManager();
    // Late-bind context pointers
    AppContext* ctx = getContext();
    if (ctx) {
        if (!_display) _display = SafePtr::resolve(ctx->display);
    }
    _display->clear();
}

// Update the menu
bool Menu::update(bool forceRedraw) {
    IInputManager* inputManager = getInputManager();
    bool redraw = forceRedraw;
    
    // Handle encoder rotation
    const auto encoderDirection = inputManager ? inputManager->getEncoderDirection() : IInputManager::EncoderDirection::None;
    if (encoderDirection != IInputManager::EncoderDirection::None) {
        if (encoderDirection == IInputManager::EncoderDirection::Clockwise) {
            _menuIndex = (_menuIndex + 1) % _currentMenuSize;
        } else {
            _menuIndex = (_menuIndex - 1 + _currentMenuSize) % _currentMenuSize;
        }
        updateScrollOffset();
        
        // Jump selection highlight directly to the new item
        const uint8_t displayIndex = _menuIndex - _scrollOffset;
        _selectionYPos = (displayIndex + 1) * MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET;

        redraw = true;
    }


    if (redraw) {
        drawMenu();
    }

    // Handle encoder button press
    if (inputManager && inputManager->isButtonPressed()) {
        return handleMenuSelection();
    }
    return true;
}

void Menu::drawScrollbar() {
    // Only draw scrollbar if there are more items than can be displayed
    if (_currentMenuSize <= MAX_VISIBLE_ITEMS) {
        return;
    }
    
    const uint8_t displayHeight = _display->getDisplayHeight();
    const uint8_t displayWidth = _display->getDisplayWidth();
    
    // Calculate scrollbar dimensions
    const uint8_t scrollbarTrackHeight = displayHeight - (2 * SCROLLBAR_Y_MARGIN);
    // Use uint16_t to prevent overflow in multiplication before division
    const uint16_t heightCalc = (uint16_t)MAX_VISIBLE_ITEMS * scrollbarTrackHeight / _currentMenuSize;
    const uint8_t scrollbarHeight = max((uint8_t)4, (uint8_t)heightCalc);
    
    // Calculate scrollbar position based on the highlighted item
    // Safety: _currentMenuSize > MAX_VISIBLE_ITEMS (>= 5), so (_currentMenuSize - 1) >= 4
    const uint8_t maxScrollRange = scrollbarTrackHeight - scrollbarHeight;
    const uint16_t posCalc = (uint16_t)_menuIndex * maxScrollRange / (_currentMenuSize - 1);
    const uint8_t scrollbarY = SCROLLBAR_Y_MARGIN + (uint8_t)posCalc;
    
    // Calculate X position (right side of display)
    const uint8_t scrollbarX = displayWidth - SCROLLBAR_WIDTH - SCROLLBAR_X_OFFSET;
    
    // Draw the scrollbar
    _display->drawBox(scrollbarX, scrollbarY, SCROLLBAR_WIDTH, scrollbarHeight);
}

// Helper functions
void Menu::drawMenu() {
    if (!_display) return;
    _display->clearBuffer();
    _display->setFontMode(1);
    _display->setDrawColor(1);
    _display->setBitmapMode(1);
    _display->setFont(u8g2_font_t0_11_tf); // Use a font that supports UTF-8

    const uint8_t visibleEnd = min((uint8_t)(_scrollOffset + MAX_VISIBLE_ITEMS), _currentMenuSize);

    // Draw visible menu items
    for (uint8_t i = _scrollOffset; i < visibleEnd; i++) {
        const uint8_t displayIndex = i - _scrollOffset;
        const uint8_t yPos = (displayIndex + 1) * MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET;
        _display->drawUTF8(MENU_TEXT_X_OFFSET, yPos, _currentMenu[i].name);
        if (_currentMenu[i].icon != nullptr) {
            _display->drawXBMP(MENU_ICON_X_OFFSET, yPos + MENU_ICON_Y_OFFSET, MENU_ICON_WIDTH, MENU_ICON_HEIGHT, _currentMenu[i].icon);
        }
    }
    
    // Draw selection highlight at current position
    _display->setDrawColor(2);
    _display->drawRBox(MENU_SELECTION_X_OFFSET, _selectionYPos + MENU_SELECTION_Y_OFFSET, _display->getDisplayWidth() - 10, MENU_SELECTION_HEIGHT, MENU_SELECTION_RADIUS);
    _display->setDrawColor(1);

    drawScrollbar();

    _display->sendBuffer();
}

uint8_t Menu::getCurrentMenuSize() const {
    uint8_t size = 0;
    while (_currentMenu[size].name != nullptr) size++;
    return size;
}

void Menu::updateScrollOffset() {
    if (_currentMenuSize <= MAX_VISIBLE_ITEMS) {
        _scrollOffset = 0;
        return;
    }
    
    // Scroll down if selected item is below visible area
    if (_menuIndex >= _scrollOffset + MAX_VISIBLE_ITEMS) {
        _scrollOffset = _menuIndex - MAX_VISIBLE_ITEMS + 1;
    }
    // Scroll up if selected item is above visible area
    else if (_menuIndex < _scrollOffset) {
        _scrollOffset = _menuIndex;
    }
}

bool Menu::handleMenuSelection() {
    MenuItem* selectedItem = &_currentMenu[_menuIndex];
    if (selectedItem->subMenu != nullptr) {
        _currentMenu = selectedItem->subMenu;
        _menuIndex = 0;
        _scrollOffset = 0;
        _currentMenuSize = getCurrentMenuSize();
        _selectionYPos = MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET;
        drawMenu();
        DEBUG_PRINTLN("Submenu selected");
    } else if (selectedItem->action != nullptr && _menuActions != nullptr) {
        (_menuActions->*(selectedItem->action))();
        DEBUG_PRINTLN("Action executed");
        return false;
    }
    return true;
}

void Menu::setCurrentMenu(MenuItem* menu) {
    if (!menu) return;
    _currentMenu = menu;
    _menuIndex = 0;
    _scrollOffset = 0;
    _currentMenuSize = getCurrentMenuSize();
    _selectionYPos = MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET;
    drawMenu();
}
