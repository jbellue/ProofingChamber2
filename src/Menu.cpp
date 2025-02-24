#include "Menu.h"

// Menu definitions
Menu::MenuItem Menu::mainMenu[] = {
    {MENU_PROOF_NOW, "Proof now", nullptr, &Menu::proofNowAction},
    {MENU_COOL_NOW, "Cool now", coolMenu, nullptr},
    {MENU_SETTINGS, "Settings", settingsMenu, nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr} // End of menu
};

Menu::MenuItem Menu::coolMenu[] = {
    {MENU_PROOF_IN, "Proof in...", nullptr, &Menu::proofInAction},
    {MENU_PROOF_AT, "Proof at...", nullptr, &Menu::proofAtAction},
    {MENU_BACK, "Back", mainMenu, nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr} // End of menu
};

Menu::MenuItem Menu::settingsMenu[] = {
    {MENU_HOT, "Hot", hotMenu, nullptr},
    {MENU_COLD, "Cold", coldMenu, nullptr},
    {MENU_CLOCK, "Clock", nullptr, &Menu::clockAction},
    {MENU_BACK, "Back", mainMenu, nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr} // End of menu
};

Menu::MenuItem Menu::hotMenu[] = {
    {MENU_TARGET_TEMP, "Target Temperature", nullptr, &Menu::adjustHotTargetTemp},
    {MENU_LOWER_LIMIT, "Lower limit", nullptr, &Menu::adjustHotLowerLimit},
    {MENU_HIGHER_LIMIT, "Higher limit", nullptr, &Menu::adjustHotHigherLimit},
    {MENU_BACK, "Back", settingsMenu, nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr} // End of menu
};

Menu::MenuItem Menu::coldMenu[] = {
    {MENU_TARGET_TEMP, "Target Temperature", nullptr, &Menu::adjustColdTargetTemp},
    {MENU_LOWER_LIMIT, "Lower limit", nullptr, &Menu::adjustColdLowerLimit},
    {MENU_HIGHER_LIMIT, "Higher limit", nullptr, &Menu::adjustColdHigherLimit},
    {MENU_BACK, "Back", settingsMenu, nullptr},
    {MENU_LAST_ITEM, nullptr, nullptr, nullptr} // End of menu
};

// Constructor
Menu::Menu(Storage& storage, ESP32Encoder& encoder, int encoderSWPin)
    : _storage(storage), _encoder(encoder),
        _encoderSWPin(encoderSWPin), _currentMenu(mainMenu), _menuIndex(0), _oldPosition(-999),
        _lastButtonState(HIGH), _buttonState(HIGH), _lastDebounceTime(0), _display(U8G2_R0, U8X8_PIN_NONE) {
    pinMode(_encoderSWPin, INPUT_PULLUP); // Initialize the encoder switch pin
}

// Initialize the menu
void Menu::begin() {
    _currentMenu = mainMenu;
    _menuIndex = 0;
    drawMenu(_currentMenu, _menuIndex);
    Serial.println("Menu initialized");
}

// Update the menu
void Menu::update() {
    // Handle encoder rotation
    int64_t newPosition = _encoder.getCount() / 4;
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
    _display.setCursor(0, 0);
    _display.println("Proofing now...");
    _display.sendBuffer();
    delay(1000);
}

void Menu::proofInAction() {
    _display.clear();
    _display.setCursor(0, 0);
    _display.println("Proof in...");
    _display.sendBuffer();
    delay(1000);
}

void Menu::proofAtAction() {
    _display.clear();
    _display.setCursor(0, 0);
    _display.println("Proof at...");
    _display.sendBuffer();
    delay(1000);
}

void Menu::clockAction() {
    _display.clear();
    _display.setCursor(0, 0);
    _display.println("Clock settings...");
    _display.sendBuffer();
    delay(1000);
}

void Menu::adjustHotTargetTemp() {
    adjustValue("Target Temp", "/hot/target_temp.txt");
}

void Menu::adjustHotLowerLimit() {
    adjustValue("Lower Limit", "/hot/lower_limit.txt");
}

void Menu::adjustHotHigherLimit() {
    adjustValue("Higher Limit", "/hot/higher_limit.txt");
}

void Menu::adjustColdTargetTemp() {
    adjustValue("Target Temp", "/cold/target_temp.txt");
}

void Menu::adjustColdLowerLimit() {
    adjustValue("Lower Limit", "/cold/lower_limit.txt");
}

void Menu::adjustColdHigherLimit() {
    adjustValue("Higher Limit", "/cold/higher_limit.txt");
}

void Menu::adjustValue(const char* title, const char* path) {
    int64_t newPosition = _encoder.getCount() / 4;
    int64_t oldPosition = newPosition;

    // Load the initial value from storage
    int value = _storage.readIntFromFile(path, 0);
    Serial.println(value);

    while (true) {
        // Display the current value
        _display.clear();
        _display.setCursor(0, 0);
        _display.println(title);
        _display.setCursor(0, 10);
        _display.print("Value: ");
        _display.println(value);
        _display.sendBuffer();

        // Handle encoder rotation
        newPosition = _encoder.getCount() / 4;
        if (newPosition != oldPosition) {
            if (newPosition > oldPosition) {
                value++; // Increment value
            } else {
                value--; // Decrement value
            }
            oldPosition = newPosition;
        }

        // Handle encoder button press to confirm and save
        if (isButtonPressed()) {
            // Save the new value to storage
            _storage.writeIntToFile(path, value);
            break; // Exit the adjustment loop
        }
    }
}

// Helper functions
void Menu::drawMenu(MenuItem* menu, int index) {
    _display.clearBuffer();
    _display.setFontMode(1);
    _display.setBitmapMode(1);
    _display.setFont(u8g2_font_t0_11_tr);
    for (int i = 0; menu[i].name != nullptr; i++) {
        _display.drawUTF8(3, i * 13 + 13, menu[i].name);
        if (i == index) {
            _display.setDrawColor(2);
            _display.drawRBox(0, i * 13 + 13, 118, 13, 1);
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