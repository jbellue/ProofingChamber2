#include <WiFiManager.h>
#include "DebugUtils.h"
#include "Menu.h"
#include "icons.h"

// Constructor
Menu::Menu(U8G2_SH1106_128X64_NONAME_F_HW_I2C* display, InputManager* inputManager) :
    display(display),
    inputManager(inputManager),
    _currentMenu(nullptr),
    _currentState(STATE_IDLE)
{}

// Initialize the menu
void Menu::begin(MenuItem* mainMenu) {
    display->clearBuffer();
    display->setFont(u8g2_font_t0_11_tf);
    inputManager->begin();
    _currentMenu = mainMenu;
    _menuIndex = 0;
    drawMenu(_currentMenu, _menuIndex);
    DEBUG_PRINTLN("Menu Began");
}

// Update the menu
void Menu::update() {
    inputManager->update();

    switch(_currentState) {
        case STATE_IDLE:
            handleMenuNavigation();
            break;
        case STATE_ADJUST_VALUE:
            handleAdjustValue();
            break;
        case STATE_ADJUST_TIME:
            handleAdjustTime();
            break;
    }
}

void Menu::startAdjustValue(const char* title, const char* path) {
    _currentState = STATE_ADJUST_VALUE;
    _currentTitle = title;
    _currentPath = path;
    _currentValue = _storage.readIntFromFile(path, 0); // Load initial value
    _oldPosition = inputManager->getEncoderPosition(); // Reset encoder position

    // Update the display immediately
    display->clear();
    _valueY = drawTitle();
    updateAdjustValueDisplay();
}

void Menu::startSetTime(const char* title, const uint8_t startH, const uint8_t startM) {
    _currentState = STATE_ADJUST_TIME;
    _currentTitle = title;
    _currentHours = startH;
    _currentMinutes = startM;
    _adjustingHours = true;
    _oldPosition = inputManager->getEncoderPosition(); // Reset encoder position

    // Update the display immediately
    display->clear();
    _valueY = drawTitle();
    updateAdjustTimeDisplay();
}

void Menu::handleMenuNavigation() {
    // Handle encoder rotation
    int64_t newPosition = inputManager->getEncoderPosition();
    if (newPosition != _oldPosition) {
        if (newPosition > _oldPosition) {
            _menuIndex = (_menuIndex + 1) % getMenuSize(_currentMenu);
        } else {
            _menuIndex = (_menuIndex - 1 + getMenuSize(_currentMenu)) % getMenuSize(_currentMenu);
        }
        _oldPosition = newPosition;
        drawMenu(_currentMenu, _menuIndex);
    }

    // Handle encoder button press
    if (inputManager->isButtonPressed()) {
        handleMenuSelection();
    }
}

void Menu::handleAdjustValue() {
    // Handle encoder rotation
    int64_t newPosition = inputManager->getEncoderPosition();
    if (newPosition != _oldPosition) {
        _currentValue += (newPosition > _oldPosition) ? 1 : -1;
        _oldPosition = newPosition;
        updateAdjustValueDisplay();
    }

    // Handle encoder button press to confirm and save
    if (inputManager->isButtonPressed()) {
        _storage.writeIntToFile(_currentPath, _currentValue);
        _currentState = STATE_IDLE; // Return to idle state
        drawMenu(_currentMenu, _menuIndex); // Redraw the menu
    }
}

void Menu::handleAdjustTime() {
    //TODO allow tomorrow
    // Handle encoder rotation
    int64_t newPosition = inputManager->getEncoderPosition();
    if (newPosition != _oldPosition) {
        if (_adjustingHours) {
            _currentHours += (newPosition > _oldPosition) ? 1 : -1;
            if (_currentHours < 0) {
                _currentHours = 23;
                if (_currentDays > 0) _currentDays -= 1;
            }
            if (_currentHours > 23) {
                _currentHours = 0;
                _currentDays += 1;
            }
        } else {
            _currentMinutes += (newPosition > _oldPosition) ? 1 : -1;
            if (_currentMinutes < 0) _currentMinutes = 59;
            if (_currentMinutes > 59) _currentMinutes = 0;
        }
        _oldPosition = newPosition;
        updateAdjustTimeDisplay();
    }

    // Handle encoder button press to switch between hours and minutes, or confirm and save
    if (inputManager->isButtonPressed()) {
        if (_adjustingHours) {
            _adjustingHours = false; // Switch to adjusting minutes
            // Update the display immediately to reflect the change
            updateAdjustTimeDisplay();
        } else {
            DEBUG_PRINTLN("Set time: " + String(_currentHours) + ":" + String(_currentMinutes));
            _currentState = STATE_IDLE; // Return to idle state
            drawMenu(_currentMenu, _menuIndex); // Redraw the menu
        }
    }
}

void Menu::updateAdjustValueDisplay() {
    display->setFont(u8g2_font_ncenB18_tn);
    char buffer[4] = {'\0'};
    sprintf(buffer, "%d", _currentValue);
    const uint8_t degreeSymbolWidth = 8;
    const uint8_t valueWidth = display->getStrWidth(buffer);
    const uint8_t valueX = (display->getDisplayWidth() - valueWidth - degreeSymbolWidth) / 2; // Calculate the X position to center the value
    const uint8_t valueY = _valueY + 2;

    // Only redraw the value and degree symbol
    display->setDrawColor(0); // Clear the previous value
    const uint8_t clearWidth = valueWidth + degreeSymbolWidth + 1; // Add buffer to ensure full clearing
    const uint8_t clearHeight = display->getAscent() - display->getDescent() + 4; // Add buffer to ensure full clearing
    display->drawBox(valueX, valueY - display->getAscent(), clearWidth, clearHeight);
    display->setDrawColor(1); // Draw the new value
    display->drawStr(valueX, valueY, buffer);

    // Draw the custom degree symbol just after the value
    const uint8_t symbolX = valueX + valueWidth;
    const uint8_t symbolY = valueY - display->getAscent();
    display->drawXBMP(symbolX, symbolY, degreeSymbolWidth, degreeSymbolWidth, degreeSymbol);

    display->sendBuffer();
}

void Menu::updateAdjustTimeDisplay() {
    display->setFont(u8g2_font_ncenB18_tn);
    char timeBuffer[6]; // Buffer for the time string (e.g., "12:34")
    sprintf(timeBuffer, "%02d:%02d", _currentHours, _currentMinutes); // Format the time as HH:MM
    const uint8_t timeWidth = display->getStrWidth("00:00"); // Measure the width of a default time string
    const uint8_t timeX = (display->getDisplayWidth() - timeWidth) / 2; // Calculate the X position to center the time
    const uint8_t timeY = _valueY + 2;

    // Only redraw the time string
    display->setDrawColor(0); // Clear the previous time
    const uint8_t clearWidth = display->getWidth();
    const uint8_t clearHeight = display->getAscent() - display->getDescent() + 6; // Add buffer to ensure full clearing
    display->drawBox(timeX, timeY - display->getAscent(), clearWidth, clearHeight);
    display->setDrawColor(1); // Draw the new time
    display->drawStr(timeX, timeY, timeBuffer);

    // Highlight the currently adjusted value (hours or minutes)
    if (_adjustingHours) {
        // Highlight hours (first two characters)
        display->drawHLine(timeX, timeY + 2, display->getStrWidth("00"));
    } else {
        // Highlight minutes (last two characters)
        display->drawHLine(timeX + display->getStrWidth("00:"), timeY + 2, display->getStrWidth("00"));
    }

    if(_currentDays > 0) {
        display->setFont(u8g2_font_ncenB12_tr);
        sprintf(timeBuffer, "+%dj", _currentDays); // Format the days as +1j
        display->drawStr(timeX + timeWidth + 2, timeY - 4, timeBuffer);
    }

    display->sendBuffer();
}

uint8_t Menu::drawTitle(const uint8_t startY) {
    display->setFont(u8g2_font_t0_11_tf);
    const uint8_t lineHeight = display->getAscent() - display->getDescent() + 2; // Line height (font height + spacing)
    const uint8_t displayWidth = display->getDisplayWidth(); // Get the display width

    // Split the title into lines based on EOL or CR
    char titleCopy[100]; // Copy the title to a mutable buffer
    strncpy(titleCopy, _currentTitle, sizeof(titleCopy));
    titleCopy[sizeof(titleCopy) - 1] = '\0'; // Ensure null termination

    char* line = strtok(titleCopy, "\n"); // Split the title into lines
    uint8_t currentY = startY; // Initial Y position for drawing

    while (line != nullptr) {
        // Draw the current line centered on the display
        display->drawUTF8((displayWidth - display->getStrWidth(line)) / 2, currentY, line);
        currentY += lineHeight; // Move Y position down

        // Get the next line
        line = strtok(nullptr, "\n");
    }
    return currentY + lineHeight;
}

void Menu::setCurrentTitle(const char * title) {
    _currentTitle = title;
}

// Helper functions
void Menu::drawMenu(MenuItem* menu, const uint8_t index) {
    display->clearBuffer();
    display->setFontMode(1);
    display->setBitmapMode(1);
    display->setFont(u8g2_font_t0_11_tf); // Use a font that supports UTF-8
    for (uint8_t i = 0; menu[i].name != nullptr; i++) {
        const uint8_t yPos = (i + 1) * 16 - 3;
        display->drawUTF8(16, yPos, menu[i].name);
        if (menu[i].icon != nullptr) {
            display->drawXBMP(3, yPos - 9, 10, 10, menu[i].icon);
        }
        if (i == index) {
            display->setDrawColor(2);
            display->drawRBox(0, yPos - 12, 128, 15, 1);
        }
    }
    display->sendBuffer();
    DEBUG_PRINTLN("Menu drawn");
}

uint8_t Menu::getMenuSize(MenuItem* menu) {
    uint8_t size = 0;
    while (menu[size].name != nullptr) size++;
    return size;
}

void Menu::handleMenuSelection() {
    MenuItem* selectedItem = &_currentMenu[_menuIndex];
    if (selectedItem->subMenu != nullptr) {
        _currentMenu = selectedItem->subMenu;
        _menuIndex = 0;
        drawMenu(_currentMenu, _menuIndex);
        DEBUG_PRINTLN("Submenu selected");
    } else if (selectedItem->action != nullptr) {
        selectedItem->action();

        if (_currentState == STATE_IDLE) {
            // Only redraw the menu if we're still in the idle state
            drawMenu(_currentMenu, _menuIndex);
        }
        DEBUG_PRINTLN("Action executed");
    }
}