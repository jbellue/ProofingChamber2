#include "DebugUtils.h"
#include "Menu.h"
#include "MenuItems.h"
#include "SafePtr.h"
#include "screens/controllers/ProofingController.h"
#include "icons.h"
#include "screens/BaseController.h"
#include <cmath>

// Constructor
Menu::Menu(AppContext* ctx, MenuActions* menuActions) :
    BaseController(ctx),
    _menuActions(menuActions),
    _display(nullptr),
    _currentMenu(nullptr),
    _menuIndex(0),
    _scrollOffset(0),
    _scrollOffsetFloat(0),
    _targetScrollOffset(0)
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
        _currentMenuSize = getCurrentMenuSize();
        // Initialize scroll to position first item at SELECTION_POSITION
        _targetScrollOffset = static_cast<float>(_menuIndex) - SELECTION_POSITION;
        _scrollOffsetFloat = _targetScrollOffset;
        _scrollOffset = static_cast<int16_t>(floorf(_scrollOffsetFloat));
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
    
    // Check if we're currently animating
    const bool isAnimating = fabsf(_targetScrollOffset - _scrollOffsetFloat) > ANIMATION_CONVERGENCE_THRESHOLD;
    
    // Process encoder input - only one step at a time for smooth animation
    // If we're still animating, don't process new input yet
    bool indexChanged = false;
    if (!isAnimating) {
        const auto encoderDirection = inputManager ? inputManager->getEncoderDirection() : IInputManager::EncoderDirection::None;
        if (encoderDirection != IInputManager::EncoderDirection::None) {
            if (encoderDirection == IInputManager::EncoderDirection::Clockwise) {
                _menuIndex = (_menuIndex + 1) % _currentMenuSize;
            } else {
                _menuIndex = (_menuIndex - 1 + _currentMenuSize) % _currentMenuSize;
            }
            indexChanged = true;
        }
    }
    
    // If menu index changed, calculate new target scroll offset
    // The goal is to position the selected item at SELECTION_POSITION
    if (indexChanged) {
        // Always position selected item at SELECTION_POSITION
        // Items will wrap around using modulo arithmetic in drawing code
        _targetScrollOffset = _menuIndex - SELECTION_POSITION;
        
        redraw = true;
    }

    // Animate scroll offset towards target
    if (fabsf(_targetScrollOffset - _scrollOffsetFloat) > ANIMATION_CONVERGENCE_THRESHOLD) {
        _scrollOffsetFloat += (_targetScrollOffset - _scrollOffsetFloat) * ANIMATION_SPEED;
        redraw = true;
    } else if (indexChanged) {
        // Just reached target, snap to it
        _scrollOffsetFloat = _targetScrollOffset;
        redraw = true;
    }
    
    // Update integer scroll offset (can be negative for circular scrolling)
    // Use floor (not round) to get the integer part - this ensures scrollFraction is always positive
    const int16_t newScrollOffset = static_cast<int16_t>(floorf(_scrollOffsetFloat));
    if (newScrollOffset != _scrollOffset) {
        _scrollOffset = newScrollOffset;
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
    // Scrollbar height represents the proportion of visible items to total items
    const uint16_t heightCalc = min(static_cast<uint16_t>(MAX_VISIBLE_ITEMS * scrollbarTrackHeight / _currentMenuSize), static_cast<uint16_t>(20));
    const uint8_t scrollbarHeight = min(scrollbarTrackHeight, max(static_cast<uint8_t>(4), static_cast<uint8_t>(heightCalc)));
    
    // Calculate scrollbar position based on the highlighted item
    // Safety: _currentMenuSize > MAX_VISIBLE_ITEMS (>= 5), so (_currentMenuSize - 1) >= 4
    const uint8_t maxScrollRange = scrollbarTrackHeight - scrollbarHeight;
    const uint16_t posCalc = (uint16_t)_menuIndex * maxScrollRange / (_currentMenuSize - 1);
    const uint8_t scrollbarY = SCROLLBAR_Y_MARGIN + (uint8_t)posCalc;
    
    // Calculate X position (right side of display)
    const uint8_t scrollbarX = displayWidth - SCROLLBAR_WIDTH - SCROLLBAR_X_OFFSET;
    
    // Draw the scrollbar
    _display->drawBox(scrollbarX, scrollbarY, SCROLLBAR_WIDTH, scrollbarHeight);
    _display->drawVLine(scrollbarX + 1, SCROLLBAR_Y_MARGIN, scrollbarTrackHeight);
}

// Helper functions
void Menu::drawMenu() {
    if (!_display) return;
    _display->clearBuffer();
    _display->setFontMode(1);
    _display->setDrawColor(1);
    _display->setBitmapMode(1);
    _display->setFont(u8g2_font_t0_11_tf); // Use a font that supports UTF-8

    const float scrollFraction = _scrollOffsetFloat - _scrollOffset;
    const int16_t scrollPixelOffset = static_cast<int16_t>(scrollFraction * MENU_ITEM_HEIGHT);
    
    // Calculate selection box position (for alignment)
    const uint8_t displayHeight = _display->getDisplayHeight();
    const int16_t screenCenter = displayHeight / 2;
    const int16_t selectionY = screenCenter - MENU_SELECTION_Y_OFFSET - MENU_SELECTION_HEIGHT / 2;
    
    // Calculate selection box center - this is where the selected item's text baseline will be
    const int16_t selectionBoxCenter = selectionY + MENU_SELECTION_Y_OFFSET + MENU_SELECTION_HEIGHT / 2;
    
    // Draw menu items without looping
    // Show blank space above first item and below last item
    // We always draw MAX_VISIBLE_ITEMS + 1 items to handle scrolling transitions
    for (uint8_t displayIndex = 0; displayIndex <= MAX_VISIBLE_ITEMS; displayIndex++) {
        // Calculate which menu item would be at this display position
        const int16_t virtualIndex = _scrollOffset + displayIndex;
        
        // Calculate Y position relative to the selection position
        // Items above/below the selection are offset by MENU_ITEM_HEIGHT
        const int16_t offsetFromSelection = static_cast<int16_t>(displayIndex) - SELECTION_POSITION;
        const int16_t yPos = selectionBoxCenter + offsetFromSelection * MENU_ITEM_HEIGHT - scrollPixelOffset + MENU_ITEM_Y_OFFSET;
        
        // Only draw if virtualIndex is within valid menu range [0, menuSize-1]
        // This creates blank space above item 0 and below last item
        if (virtualIndex >= 0 && virtualIndex < _currentMenuSize) {
            const uint8_t menuItemIndex = static_cast<uint8_t>(virtualIndex);
            
            // Calculate the full extent of the item (icon top to text bottom with margins)
            // Icon is at yPos + MENU_ICON_Y_OFFSET (-9), height MENU_ICON_HEIGHT (10)
            // Text baseline is at yPos, extends ~2-3px below for descenders
            // Add small margins to ensure partial visibility doesn't clip items
            const int16_t itemTop = yPos + MENU_ICON_Y_OFFSET - 2;  // Icon top with margin
            const int16_t itemBottom = yPos + 4;  // Text bottom with margin for descenders
            
            // Only draw if any part of the item is within visible bounds
            if (itemBottom > 0 && itemTop < static_cast<int16_t>(_display->getDisplayHeight())) {
                _display->drawUTF8(MENU_TEXT_X_OFFSET, yPos, _currentMenu[menuItemIndex].name);
                if (_currentMenu[menuItemIndex].icon != nullptr) {
                    _display->drawXBMP(MENU_ICON_X_OFFSET, yPos + MENU_ICON_Y_OFFSET, MENU_ICON_WIDTH, MENU_ICON_HEIGHT, _currentMenu[menuItemIndex].icon);
                }
            }
        }
    }
    
    // Draw selection highlight at screen center
    _display->setDrawColor(2);
    _display->drawRBox(MENU_SELECTION_X_OFFSET, selectionY + MENU_SELECTION_Y_OFFSET, _display->getDisplayWidth() - 10, MENU_SELECTION_HEIGHT, MENU_SELECTION_RADIUS);
    _display->setDrawColor(1);

    drawScrollbar();

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
        _currentMenuSize = getCurrentMenuSize();
        // Initialize scroll to position first item at SELECTION_POSITION
        _targetScrollOffset = static_cast<float>(_menuIndex) - SELECTION_POSITION;
        _scrollOffsetFloat = _targetScrollOffset;
        _scrollOffset = static_cast<int16_t>(floorf(_scrollOffsetFloat));
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
    _currentMenuSize = getCurrentMenuSize();
    // Initialize scroll to position first item at SELECTION_POSITION
    _targetScrollOffset = static_cast<float>(_menuIndex) - SELECTION_POSITION;
    _scrollOffsetFloat = _targetScrollOffset;
    _scrollOffset = static_cast<int16_t>(floorf(_scrollOffsetFloat));
    drawMenu();
}
