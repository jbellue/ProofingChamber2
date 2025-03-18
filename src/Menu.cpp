#include <WiFiManager.h>
#include "Menu.h"
#include "icons.h"

// Menu definitions
Menu::MenuItem Menu::mainMenu[] = {
    {MENU_PROOF_NOW, "Mettre en pousse", iconProof,    nullptr,      &Menu::proofNowAction},
    {MENU_COOL_NOW,  "Mettre en froid",  iconCool,     coolMenu,     nullptr},
    {MENU_SETTINGS,  "R\xC3\xA9glages",  iconSettings, settingsMenu, nullptr},
    {MENU_LAST_ITEM, nullptr,            nullptr,      nullptr,      nullptr} // End of menu
};

Menu::MenuItem Menu::coolMenu[] = {
    {MENU_PROOF_AT, "Pousser \xC3\xA0...", iconClock,     nullptr,  &Menu::proofAtAction},
    {MENU_PROOF_IN, "Pousser dans...",     iconHourglass, nullptr,  &Menu::proofInAction},
    {MENU_BACK,     "Retour",              iconBack,      mainMenu, nullptr},
    {MENU_LAST_ITEM, nullptr,              nullptr,       nullptr,  nullptr} // End of menu
};

Menu::MenuItem Menu::settingsMenu[] = {
    {MENU_HOT,           "Chaud",          iconHotSettings,  hotMenu,          nullptr},
    {MENU_COLD,          "Froid",          iconColdSettings, coldMenu,         nullptr},
    {MENU_MORE_SETTINGS, "Avanc\xC3\xA9s", iconSettings,     moreSettingsMenu, nullptr},
    {MENU_BACK,          "Retour",         iconBack,         mainMenu,         nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr, nullptr} // End of menu
};

Menu::MenuItem Menu::moreSettingsMenu[] = {
    {MENU_RESET_WIFI, "Reset du WiFi",  iconWiFi,  nullptr,      &Menu::resetWiFiAndReboot},
    {MENU_TIMEZONE,   "Fuseau horaire", iconClock, nullptr,      &Menu::adjustTimezone},
    {MENU_BACK,       "Retour",         iconBack,  settingsMenu, nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr, nullptr} // End of menu
};

Menu::MenuItem Menu::hotMenu[] = {
    {MENU_TARGET_TEMP,  "Temp\xC3\xA9rature vis\xC3\xA9" "e", iconTarget,       nullptr,      &Menu::adjustHotTargetTemp},
    {MENU_LOWER_LIMIT,  "Limite basse",                       iconColdSettings, nullptr,      &Menu::adjustHotLowerLimit},
    {MENU_HIGHER_LIMIT, "Limite haute",                       iconHotSettings,  nullptr,      &Menu::adjustHotHigherLimit},
    {MENU_BACK,         "Retour",                             iconBack,         settingsMenu, nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr, nullptr} // End of menu
};

Menu::MenuItem Menu::coldMenu[] = {
    {MENU_TARGET_TEMP,  "Temp\xC3\xA9rature vis\xC3\xA9" "e", iconTarget,       nullptr,      &Menu::adjustColdTargetTemp},
    {MENU_LOWER_LIMIT,  "Limite basse",                       iconColdSettings, nullptr,      &Menu::adjustColdLowerLimit},
    {MENU_HIGHER_LIMIT, "Limite haute",                       iconHotSettings,  nullptr,      &Menu::adjustColdHigherLimit},
    {MENU_BACK,         "Retour",                             iconBack,         settingsMenu, nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr, nullptr} // End of menu
};

// Constructor
Menu::Menu(DisplayManager* displayManager, InputManager* inputManager) :
    _displayManager(displayManager),
    _inputManager(inputManager),
    _currentMenu(mainMenu),
    _currentState(STATE_IDLE)
{}

// Initialize the menu
void Menu::begin() {
    _displayManager->initialize();
    _inputManager->begin();
    _currentMenu = mainMenu;
    _menuIndex = 0;
    drawMenu(_currentMenu, _menuIndex);
    Serial.println("Menu Began");
}

// Update the menu
void Menu::update() {
    _inputManager->update();

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

void Menu::handleMenuNavigation() {
    // Handle encoder rotation
    int64_t newPosition = _inputManager->getEncoderPosition();
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
    if (_inputManager->isButtonPressed()) {
        handleMenuSelection();
    }
}

void Menu::startAdjustValue(const char* title, const char* path) {
    _currentState = STATE_ADJUST_VALUE;
    _currentTitle = title;
    _currentPath = path;
    _currentValue = _storage.readIntFromFile(path, 0); // Load initial value
    _oldPosition = _inputManager->getEncoderPosition(); // Reset encoder position

    // Update the display immediately
    updateAdjustValueDisplay();
}

void Menu::startSetTime(const char* title, const uint8_t startH, const uint8_t startM) {
    _currentState = STATE_ADJUST_TIME;
    _currentTitle = title;
    _currentHours = startH;
    _currentMinutes = startM;
    _adjustingHours = true;
    _oldPosition = _inputManager->getEncoderPosition(); // Reset encoder position

    // Update the display immediately
    updateAdjustTimeDisplay();
}

void Menu::handleAdjustValue() {
    // Handle encoder rotation
    int64_t newPosition = _inputManager->getEncoderPosition();
    if (newPosition != _oldPosition) {
        _currentValue += (newPosition > _oldPosition) ? 1 : -1;
        _oldPosition = newPosition;
        updateAdjustValueDisplay();
    }

    // Handle encoder button press to confirm and save
    if (_inputManager->isButtonPressed()) {
        _storage.writeIntToFile(_currentPath, _currentValue);
        _currentState = STATE_IDLE; // Return to idle state
        drawMenu(_currentMenu, _menuIndex); // Redraw the menu
    }
}

void Menu::handleAdjustTime() {
    // Handle encoder rotation
    int64_t newPosition = _inputManager->getEncoderPosition();
    if (newPosition != _oldPosition) {
        if (_adjustingHours) {
            _currentHours += (newPosition > _oldPosition) ? 1 : -1;
            if (_currentHours < 0) _currentHours = 23;
            if (_currentHours > 23) _currentHours = 0;
        } else {
            _currentMinutes += (newPosition > _oldPosition) ? 1 : -1;
            if (_currentMinutes < 0) _currentMinutes = 59;
            if (_currentMinutes > 59) _currentMinutes = 0;
        }
        _oldPosition = newPosition;
        updateAdjustTimeDisplay();
    }

    // Handle encoder button press to switch between hours and minutes, or confirm and save
    if (_inputManager->isButtonPressed()) {
        if (_adjustingHours) {
            _adjustingHours = false; // Switch to adjusting minutes
            // Update the display immediately to reflect the change
            updateAdjustTimeDisplay();
        } else {
            Serial.println("Set time: " + String(_currentHours) + ":" + String(_currentMinutes));
            _currentState = STATE_IDLE; // Return to idle state
            drawMenu(_currentMenu, _menuIndex); // Redraw the menu
        }
    }
}

// Menu actions
void Menu::proofNowAction() {
    _displayManager->clear();
    _displayManager->drawUTF8(10, 20, "Proofing now...");
    _displayManager->sendBuffer();
    delay(1000);
}

void Menu::proofInAction() {
    startSetTime("Pousser dans...");
}

void Menu::proofAtAction() {
    struct tm timeinfo;
    int hour, minute;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time, defaulting to 0:00");
        startSetTime("Pousser \xC3\xA0...");
    }
    startSetTime("Pousser \xC3\xA0...", timeinfo.tm_hour, timeinfo.tm_min);
}

void Menu::clockAction() {
    _displayManager->clear();
    _displayManager->drawUTF8(10, 20, "Clock Settings...");
    _displayManager->sendBuffer();
    delay(1000);
}

void Menu::adjustHotTargetTemp() {
    startAdjustValue("Temp\xC3\xA9rature\n" "de chauffe vis\xC3\xA9" "e", "/hot/target_temp.txt");
}

void Menu::adjustHotLowerLimit() {
    startAdjustValue("Limite basse\n" "de chauffe", "/hot/lower_limit.txt");
}

void Menu::adjustHotHigherLimit() {
    startAdjustValue("Limite haute\n" "de chauffe", "/hot/higher_limit.txt");
}

void Menu::adjustColdTargetTemp() {
    startAdjustValue("Temp\xC3\xA9rature\n" "de froid vis\xC3\xA9" "e", "/cold/target_temp.txt");
}

void Menu::adjustColdLowerLimit() {
    startAdjustValue("Limite basse\n" "de froid", "/cold/lower_limit.txt");
}

void Menu::adjustColdHigherLimit() {
    startAdjustValue("Limite haute\n" "de froid", "/cold/higher_limit.txt");
}

void Menu::adjustTimezone() {
    _displayManager->clear();
    _displayManager->drawUTF8(10, 20, "Adjusting the timezone...");
    _displayManager->sendBuffer();
    delay(1000);}

void Menu::resetWiFiAndReboot() {
    // Reset Wi-Fi credentials
    WiFiManager wifiManager;
    wifiManager.resetSettings();

    // Notify the user
    _displayManager->clearBuffer();
    _displayManager->setFont(u8g2_font_t0_11_tf);
    _currentTitle = "Reset du Wi-Fi\n" "et red\xC3\xA9marrage";
    drawTitle(20);
    _displayManager->sendBuffer();

    // Delay to allow the message to be displayed
    delay(2000);

    // Reboot the device
    ESP.restart();
}

void Menu::updateAdjustValueDisplay() {
    _displayManager->clear();

    uint8_t currentY = drawTitle();

    _displayManager->setFont(u8g2_font_ncenB18_tn);
    char buffer[4] = {'\0'};
    sprintf(buffer, "%d", _currentValue);
    const uint8_t degreeSymbolWidth = 8;
    const uint8_t valueWidth = _displayManager->getStrWidth(buffer);
    const uint8_t valueX = (_displayManager->getDisplayWidth() - valueWidth - degreeSymbolWidth) / 2; // Calculate the X position to center the value
    const uint8_t valueY = currentY + 2;
    _displayManager->drawStr(valueX, valueY, buffer);

    // Draw the custom degree symbol just after the value
    const uint8_t symbolX = valueX + valueWidth;
    const uint8_t symbolY = valueY - _displayManager->getAscent();
    _displayManager->drawXBMP(symbolX, symbolY, degreeSymbolWidth, degreeSymbolWidth, degreeSymbol);

    _displayManager->sendBuffer();
}

void Menu::updateAdjustTimeDisplay() {
    _displayManager->clear();

    uint8_t currentY = drawTitle();

    // Display the time (HH:MM)
    _displayManager->setFont(u8g2_font_ncenB18_tn);
    char timeBuffer[6]; // Buffer for the time string (e.g., "12:34")
    sprintf(timeBuffer, "%02d:%02d", _currentHours, _currentMinutes); // Format the time as HH:MM
    const uint8_t timeWidth = _displayManager->getStrWidth("00:00"); // Measure the width of a default time string
    const uint8_t timeX = (_displayManager->getDisplayWidth() - timeWidth) / 2; // Calculate the X position to center the time
    const uint8_t timeY = currentY + 2;

    // Draw the time string
    _displayManager->drawStr(timeX, timeY, timeBuffer);

    // Highlight the currently adjusted value (hours or minutes)
    if (_adjustingHours) {
        // Highlight hours (first two characters)
        _displayManager->drawHLine(timeX, timeY + 2, _displayManager->getStrWidth("00"));
    } else {
        // Highlight minutes (last two characters)
        _displayManager->drawHLine(timeX + _displayManager->getStrWidth("00:"), timeY + 2, _displayManager->getStrWidth("00"));
    }

    _displayManager->sendBuffer();
}

uint8_t Menu::drawTitle(const uint8_t startY) {
    _displayManager->setFont(u8g2_font_t0_11_tf);
    const uint8_t lineHeight = _displayManager->getAscent() - _displayManager->getDescent() + 2; // Line height (font height + spacing)
    const uint8_t displayWidth = _displayManager->getDisplayWidth(); // Get the display width

    // Split the title into lines based on EOL or CR
    char titleCopy[100]; // Copy the title to a mutable buffer
    strncpy(titleCopy, _currentTitle, sizeof(titleCopy));
    titleCopy[sizeof(titleCopy) - 1] = '\0'; // Ensure null termination

    char* line = strtok(titleCopy, "\n"); // Split the title into lines
    uint8_t currentY = startY; // Initial Y position for drawing

    while (line != nullptr) {
        // Draw the current line centered on the display
        _displayManager->drawUTF8((displayWidth - _displayManager->getStrWidth(line)) / 2, currentY, line);
        currentY += lineHeight; // Move Y position down

        // Get the next line
        line = strtok(nullptr, "\n");
    }
    return currentY + lineHeight;
}


// Helper functions
void Menu::drawMenu(MenuItem* menu, int index) {
    _displayManager->clearBuffer();
    _displayManager->setFontMode(1);
    _displayManager->setBitmapMode(1);
    _displayManager->setFont(u8g2_font_t0_11_tf); // Use a font that supports UTF-8
    for (int i = 0; menu[i].name != nullptr; i++) {
        const uint8_t yPos = (i + 1) * 16 - 3;
        _displayManager->drawUTF8(16, yPos, menu[i].name);
        if (menu[i].icon != nullptr) {
            _displayManager->drawXBMP(3, yPos - 9, 10, 10, menu[i].icon);
        }
        if (i == index) {
            _displayManager->setDrawColor(2);
            _displayManager->drawRBox(0, yPos - 12, 128, 15, 1);
        }
    }
    _displayManager->sendBuffer();
    Serial.println("Menu drawn");
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
        Serial.println("Submenu selected");
    } else if (selectedItem->action != nullptr) {
        (this->*(selectedItem->action))();

        if (_currentState == STATE_IDLE) {
            // Only redraw the menu if we're still in the idle state
            drawMenu(_currentMenu, _menuIndex);
        }
        Serial.println("Action executed");
    } else if (selectedItem->id == MENU_BACK) {
        // Navigate back to the main menu
        _currentMenu = mainMenu;
        _menuIndex = 0;
        drawMenu(_currentMenu, _menuIndex);
        Serial.println("Back to main menu");
    }
}