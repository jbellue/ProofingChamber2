#include "Menu.h"
#include "icons.h"

#define ENCODER_CLK 2
#define ENCODER_DT  3
#define ENCODER_SW  4

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
    {MENU_HOT,   "Chaud",  iconHotSettings,  hotMenu,  nullptr},
    {MENU_COLD,  "Froid",  iconColdSettings, coldMenu, nullptr},
    {MENU_CLOCK, "Heure",  iconClock,        nullptr,  &Menu::clockAction},
    {MENU_BACK,  "Retour", iconBack,         mainMenu, nullptr},
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
Menu::Menu() :
    _storage(),
    _encoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::FOUR3),
    _encoderSWPin(ENCODER_SW),
    _currentMenu(mainMenu),
    _menuIndex(0),
    _oldPosition(0),
    _lastButtonState(HIGH),
    _buttonState(HIGH),
    _lastDebounceTime(0),
    _display(U8G2_R0, U8X8_PIN_NONE)
{
    // _display.setI2CAddress(0x3C << 1); // Set I2C address of the display
    pinMode(_encoderSWPin, INPUT_PULLUP); // Initialize the encoder switch pin
}

// Method to initialize the display
void Menu::initializeDisplay() {
    Serial.println("Calling _display.begin()...");
    if (!_display.begin()) {
        Serial.println("Display initialization failed!");
        while (1); // Halt execution if display initialization fails
    }
    Serial.println("Display begin called.");
    _display.clearBuffer();
    _display.setFont(u8g2_font_t0_11_tf);
    _display.drawStr(0, 10, "Display Initialized");
    _display.sendBuffer();
    Serial.println("Display buffer sent.");
    delay(1000);
    Serial.println("Display initialized.");
}

// Initialize the menu
void Menu::begin() {
    initializeDisplay(); // Initialize the display
    _currentMenu = mainMenu;
    _menuIndex = 0;
    drawMenu(_currentMenu, _menuIndex);
    Serial.println("Menu Began");
}

// Update the menu
void Menu::update() {
    _encoder.tick(); // Update the encoder state
    // Handle encoder rotation
    int64_t newPosition = _encoder.getPosition();
    if (newPosition != _oldPosition) {
        if (newPosition > _oldPosition) {
            _menuIndex = (_menuIndex + 1) % getMenuSize(_currentMenu);
        } else {
            _menuIndex = (_menuIndex - 1 + getMenuSize(_currentMenu)) % getMenuSize(_currentMenu);
        }
        _oldPosition = newPosition;
        drawMenu(_currentMenu, _menuIndex);
        Serial.print("Menu index: ");
        Serial.println(_menuIndex);
    }

    // Handle encoder button press with improved debounce
    if (isButtonPressed()) {
        Serial.println("Button pressed");
        handleMenuSelection();
    }
}

// Method to handle button press with debounce
bool Menu::isButtonPressed() {
    int reading = digitalRead(_encoderSWPin);

    if (reading != _lastButtonState) {
        _lastDebounceTime = millis();
    }

    if ((millis() - _lastDebounceTime) > 50) {
        if(reading != _buttonState) {
            _buttonState = reading;
            if (_buttonState == LOW) {
                return true;
            }
        }
    }

    _lastButtonState = reading;
    return false;
}

// Menu actions
void Menu::proofNowAction() {
    _display.clear();
    _display.drawUTF8(10, 20, "Proofing now...");
    _display.sendBuffer();
    delay(1000);
}

void Menu::proofInAction() {
    setTime("Pousser dans...");
}

void Menu::proofAtAction() {
    setTime("Pousser \xC3\xA0...");
}

void Menu::clockAction() {
    _display.clear();
    _display.drawUTF8(10, 20, "Clock Settings...");
    _display.sendBuffer();
    delay(1000);
}

void Menu::adjustHotTargetTemp() {
    adjustValue("Temp\xC3\xA9rature\n" "de chauffe vis\xC3\xA9" "e", "/hot/target_temp.txt");
}

void Menu::adjustHotLowerLimit() {
    adjustValue("Limite basse\n" "de chauffe", "/hot/lower_limit.txt");
}

void Menu::adjustHotHigherLimit() {
    adjustValue("Limite haute\n" "de chauffe", "/hot/higher_limit.txt");
}

void Menu::adjustColdTargetTemp() {
    adjustValue("Temp\xC3\xA9rature\n" "de froid vis\xC3\xA9" "e", "/cold/target_temp.txt");
}

void Menu::adjustColdLowerLimit() {
    adjustValue("Limite basse\n" "de froid", "/cold/lower_limit.txt");
}

void Menu::adjustColdHigherLimit() {
    adjustValue("Limite haute\n" "de froid", "/cold/higher_limit.txt");
}

void Menu::adjustValue(const char* title, const char* path) {
    int64_t encoderPosition = _encoder.getPosition();

    // Load the initial value from storage
    int value = _storage.readIntFromFile(path, 0);
    Serial.println(value);
    bool shouldUpdate = true;

    while (true) {
        _encoder.tick(); // Update the encoder state
        if (shouldUpdate) {
            updateAdjustValueDisplay(title, value);
            shouldUpdate = false;
        }

        // Handle encoder rotation
        const int64_t newEncoderPosition = _encoder.getPosition();
        if (newEncoderPosition != encoderPosition) {
            value += (newEncoderPosition > encoderPosition) ? 1 : -1; // Adjust value based on encoder direction
            encoderPosition = newEncoderPosition;
            shouldUpdate = true;
        }

        // Handle encoder button press to confirm and save
        if (isButtonPressed()) {
            // Save the new value to storage
            _storage.writeIntToFile(path, value);
            break; // Exit the adjustment loop
        }
    }
    _oldPosition = encoderPosition;
}

void Menu::updateAdjustValueDisplay(const char* title, int value) {
    _display.clear();

    uint8_t currentY = drawTitle(title);

    _display.setFont(u8g2_font_ncenB18_tn);
    char buffer[4] = {'\0'};
    sprintf(buffer, "%d", value);
    const uint8_t degreeSymbolWidth = 8;
    const uint8_t valueWidth = _display.getStrWidth(buffer);
    const uint8_t valueX = (_display.getDisplayWidth() - valueWidth - degreeSymbolWidth) / 2; // Calculate the X position to center the value
    const uint8_t valueY = currentY + 2;
    _display.drawStr(valueX, valueY, buffer);

    // Draw the custom degree symbol just after the value
    const uint8_t symbolX = valueX + valueWidth;
    const uint8_t symbolY = valueY - _display.getAscent();
    _display.drawXBMP(symbolX, symbolY, degreeSymbolWidth, degreeSymbolWidth, degreeSymbol);

    _display.sendBuffer();
}


void Menu::setTime(const char* title) {
    int64_t encoderPosition = _encoder.getPosition();

    int hours = 0;
    int minutes = 0;
    bool shouldUpdate = true;
    bool adjustingHours = true; // Start by adjusting hours

    while (true) {
        _encoder.tick(); // Update the encoder state
        if (shouldUpdate) {
            updateAdjustTimeDisplay(title, hours, minutes, adjustingHours);
            shouldUpdate = false;
        }

        // Handle encoder rotation
        const int64_t newEncoderPosition = _encoder.getPosition();
        if (newEncoderPosition != encoderPosition) {
            if (adjustingHours) {
                hours += (newEncoderPosition > encoderPosition) ? 1 : -1; // Adjust hours
                if (hours < 0) hours = 23; // Wrap around (0-23)
                if (hours > 23) hours = 0;
            } else {
                minutes += (newEncoderPosition > encoderPosition) ? 1 : -1; // Adjust minutes
                if (minutes < 0) minutes = 59; // Wrap around (0-59)
                if (minutes > 59) minutes = 0;
            }
            encoderPosition = newEncoderPosition;
            shouldUpdate = true;
        }

        // Handle encoder button press to switch between hours and minutes, or confirm and save
        if (isButtonPressed()) {
            if (adjustingHours) {
                adjustingHours = false; // Switch to adjusting minutes
            } else {
                Serial.println("Set time: " + String(hours) + ":" + String(minutes));
                break; // Exit the adjustment loop
            }
            shouldUpdate = true;
        }
    }
    _oldPosition = encoderPosition;
}

void Menu::updateAdjustTimeDisplay(const char* title, int hours, int minutes, bool adjustingHours) {
    _display.clear();

    uint8_t currentY = drawTitle(title);

    // Display the time (HH:MM)
    _display.setFont(u8g2_font_ncenB18_tn);
    char timeBuffer[6]; // Buffer for the time string (e.g., "12:34")
    sprintf(timeBuffer, "%02d:%02d", hours, minutes); // Format the time as HH:MM
    const uint8_t timeWidth = _display.getStrWidth("00:00"); // Measure the width of a default time string
    const uint8_t timeX = (_display.getDisplayWidth() - timeWidth) / 2; // Calculate the X position to center the time
    const uint8_t timeY = currentY + 2;

    // Draw the time string
    _display.drawStr(timeX, timeY, timeBuffer);

    // Highlight the currently adjusted value (hours or minutes)
    if (adjustingHours) {
        // Highlight hours (first two characters)
        _display.drawHLine(timeX, timeY + 2, _display.getStrWidth("00"));
    } else {
        // Highlight minutes (last two characters)
        _display.drawHLine(timeX + _display.getStrWidth("00:"), timeY + 2, _display.getStrWidth("00"));
    }

    _display.sendBuffer();
}

uint8_t Menu::drawTitle(const char* title, const uint8_t startY) {
    _display.setFont(u8g2_font_t0_11_tf);
    const uint8_t lineHeight = _display.getAscent() - _display.getDescent() + 2; // Line height (font height + spacing)
    const uint8_t displayWidth = _display.getDisplayWidth(); // Get the display width

    // Split the title into lines based on EOL or CR
    char titleCopy[100]; // Copy the title to a mutable buffer
    strncpy(titleCopy, title, sizeof(titleCopy));
    titleCopy[sizeof(titleCopy) - 1] = '\0'; // Ensure null termination

    char* line = strtok(titleCopy, "\n"); // Split the title into lines
    uint8_t currentY = startY; // Initial Y position for drawing

    while (line != nullptr) {
        // Draw the current line centered on the display
        _display.drawUTF8((displayWidth - _display.getStrWidth(line)) / 2, currentY, line);
        currentY += lineHeight; // Move Y position down

        // Get the next line
        line = strtok(nullptr, "\n");
    }
    return currentY + lineHeight;
}


// Helper functions
void Menu::drawMenu(MenuItem* menu, int index) {
    _display.clearBuffer();
    _display.setFontMode(1);
    _display.setBitmapMode(1);
    _display.setFont(u8g2_font_t0_11_tf); // Use a font that supports UTF-8
    for (int i = 0; menu[i].name != nullptr; i++) {
        const uint8_t yPos = (i + 1) * 16 - 3;
        _display.drawUTF8(16, yPos, menu[i].name);
        if (menu[i].icon != nullptr) {
            _display.drawXBMP(3, yPos - 9, 10, 10, menu[i].icon);
        }
        if (i == index) {
            _display.setDrawColor(2);
            _display.drawRBox(0, yPos - 12, 128, 15, 1);
        }
    }
    _display.sendBuffer();
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
        drawMenu(_currentMenu, _menuIndex);
        Serial.println("Action executed");
    } else if (selectedItem->id == MENU_BACK) {
        _currentMenu = mainMenu;
        _menuIndex = 0;
        drawMenu(_currentMenu, _menuIndex);
        Serial.println("Back to main menu");
    }
}