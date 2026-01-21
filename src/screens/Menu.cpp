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
        _scrollOffset = 0;
        _currentMenuSize = getCurrentMenuSize();
        _scrollOffsetFloat = 0;
        _targetScrollOffset = 0;
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
    
    // Process ALL pending encoder steps before animating
    // This ensures no steps are missed
    bool indexChanged = false;
    while (true) {
        const auto encoderDirection = inputManager ? inputManager->getEncoderDirection() : IInputManager::EncoderDirection::None;
        if (encoderDirection == IInputManager::EncoderDirection::None) {
            break;  // No more steps to process
        }
        
        if (encoderDirection == IInputManager::EncoderDirection::Clockwise) {
            _menuIndex = (_menuIndex + 1) % _currentMenuSize;
        } else {
            _menuIndex = (_menuIndex - 1 + _currentMenuSize) % _currentMenuSize;
        }
        indexChanged = true;
    }
    
    // If menu index changed, calculate new target scroll offset
    if (indexChanged) {
        // Calculate target scroll offset based on menu position
        if (_currentMenuSize <= MAX_VISIBLE_ITEMS) {
            // All items fit on screen - no scrolling needed
            _targetScrollOffset = 0;
        } else {
            // Implement smart scrolling:
            // - First item stays at top (scroll offset 0)
            // - Last item stays at bottom (scroll offset so last item is at bottom)
            // - Middle items scroll to keep selection visible
            
            if (_menuIndex == 0) {
                // First item - keep at top
                _targetScrollOffset = 0;
            } else if (_menuIndex >= _currentMenuSize - 1) {
                // Last item - keep at bottom
                _targetScrollOffset = _currentMenuSize - MAX_VISIBLE_ITEMS;
            } else {
                // Middle items - scroll to keep selection in view
                // Try to keep selection in the middle of the screen if possible
                const uint8_t preferredPosition = (MAX_VISIBLE_ITEMS - 1) / 2;
                
                if (_menuIndex < preferredPosition) {
                    // Near the start - don't scroll past 0
                    _targetScrollOffset = 0;
                } else if (_menuIndex >= _currentMenuSize - preferredPosition - 1) {
                    // Near the end - don't scroll past max
                    _targetScrollOffset = _currentMenuSize - MAX_VISIBLE_ITEMS;
                } else {
                    // Middle section - center the selection
                    _targetScrollOffset = _menuIndex - preferredPosition;
                }
            }
        }
        
        redraw = true;
    }

    // Animate scroll offset towards target
    if (fabsf(_targetScrollOffset - _scrollOffsetFloat) > ANIMATION_CONVERGENCE_THRESHOLD) {
        _scrollOffsetFloat += (_targetScrollOffset - _scrollOffsetFloat) * ANIMATION_SPEED;
        redraw = true;
    } else {
        // Snap to target when very close
        _scrollOffsetFloat = _targetScrollOffset;
    }
    
    // Update integer scroll offset
    const uint8_t newScrollOffset = static_cast<uint8_t>(max(0.0f, min(_scrollOffsetFloat + 0.5f, 255.0f)));
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

    const uint8_t visibleEnd = min(static_cast<uint8_t>(_scrollOffset + MAX_VISIBLE_ITEMS), _currentMenuSize);

    // Draw visible menu items with smooth scrolling
    // Items are offset by the fractional part of the scroll offset
    const float scrollFraction = _scrollOffsetFloat - _scrollOffset;
    const int16_t scrollPixelOffset = static_cast<int16_t>(scrollFraction * MENU_ITEM_HEIGHT);
    
    for (uint8_t i = _scrollOffset; i < visibleEnd; i++) {
        const uint8_t displayIndex = i - _scrollOffset;
        const int16_t yPos = (displayIndex + 1) * MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET - scrollPixelOffset;
        
        // Only draw if within visible bounds
        if (yPos > -MENU_ITEM_HEIGHT && yPos < static_cast<int16_t>(_display->getDisplayHeight())) {
            _display->drawUTF8(MENU_TEXT_X_OFFSET, yPos, _currentMenu[i].name);
            if (_currentMenu[i].icon != nullptr) {
                _display->drawXBMP(MENU_ICON_X_OFFSET, yPos + MENU_ICON_Y_OFFSET, MENU_ICON_WIDTH, MENU_ICON_HEIGHT, _currentMenu[i].icon);
            }
        }
    }
    
    // Draw one extra item above if scrolling
    if (_scrollOffset > 0 && scrollFraction > SCROLL_RENDER_THRESHOLD) {
        const uint8_t i = _scrollOffset - 1;
        const int16_t yPos = MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET - scrollPixelOffset;
        if (yPos > -MENU_ITEM_HEIGHT && yPos < static_cast<int16_t>(_display->getDisplayHeight())) {
            _display->drawUTF8(MENU_TEXT_X_OFFSET, yPos, _currentMenu[i].name);
            if (_currentMenu[i].icon != nullptr) {
                _display->drawXBMP(MENU_ICON_X_OFFSET, yPos + MENU_ICON_Y_OFFSET, MENU_ICON_WIDTH, MENU_ICON_HEIGHT, _currentMenu[i].icon);
            }
        }
    }
    
    // Draw one extra item below if scrolling
    if (visibleEnd < _currentMenuSize && scrollFraction > SCROLL_RENDER_THRESHOLD) {
        const uint8_t i = visibleEnd;
        const uint8_t displayIndex = i - _scrollOffset;
        const int16_t yPos = (displayIndex + 1) * MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET - scrollPixelOffset;
        if (yPos > -MENU_ITEM_HEIGHT && yPos < static_cast<int16_t>(_display->getDisplayHeight())) {
            _display->drawUTF8(MENU_TEXT_X_OFFSET, yPos, _currentMenu[i].name);
            if (_currentMenu[i].icon != nullptr) {
                _display->drawXBMP(MENU_ICON_X_OFFSET, yPos + MENU_ICON_Y_OFFSET, MENU_ICON_WIDTH, MENU_ICON_HEIGHT, _currentMenu[i].icon);
            }
        }
    }
    
    // Draw selection highlight at fixed position relative to displayed items
    // The highlight doesn't move - items scroll underneath it
    const float displayIndex = _menuIndex - _scrollOffsetFloat;
    const int16_t selectionY = static_cast<int16_t>((displayIndex + 1) * MENU_ITEM_HEIGHT + MENU_ITEM_Y_OFFSET + 0.5f) - scrollPixelOffset;
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
        _scrollOffset = 0;
        _currentMenuSize = getCurrentMenuSize();
        _scrollOffsetFloat = 0;
        _targetScrollOffset = 0;
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
    _scrollOffsetFloat = 0;
    _targetScrollOffset = 0;
    drawMenu();
}
