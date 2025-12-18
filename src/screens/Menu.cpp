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
    // Ensure dynamic menus (like timezones) are initialized
    initializeAllMenus();
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

// Draws a small filled arrow (up or down) using horizontal lines for pixel-perfect symmetry
void Menu::drawTriangle(const bool up, const uint8_t centerX, const uint8_t topY, const uint8_t height) {
    for (uint8_t row = 0; row < height; ++row) {
        const uint8_t y = up ? (topY + row)          // grow downward for up arrow
                              : (topY + (height - 1 - row)); // grow upward for down arrow
        const uint8_t startX = centerX - row;
        const uint8_t width = row * 2 + 1;
        _display->drawHLine(startX, y, width);
    }
}

void Menu::drawNavigationHints(const uint8_t visibleEnd) {
    const uint8_t arrowXPosition = _display->getDisplayWidth() - SCROLL_ARROW_X_OFFSET;

    if (_scrollOffset > 0) {
        // Up arrow at top
        drawTriangle(true, arrowXPosition, SCROLL_ARROW_Y_OFFSET, TRIANGLE_HEIGHT);
    }
    if (visibleEnd < _currentMenuSize) {
        // Down arrow at bottom
        drawTriangle(false, arrowXPosition, _display->getDisplayHeight() - TRIANGLE_HEIGHT - SCROLL_ARROW_Y_OFFSET, TRIANGLE_HEIGHT);
    }
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

    drawNavigationHints(visibleEnd);

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
