#include "AdjustValue.h"
#include "DebugUtils.h"
#include "icons.h"

AdjustValue::AdjustValue(DisplayManager* _display, InputManager* inputManager) :
    _display(_display),
    _inputManager(inputManager)
{}

void AdjustValue::begin(const char* title, const char* path) {
    beginImpl(title, path);
}

void AdjustValue::beginImpl(const char* title, const char* path) {
    _title = title;
    _path = path;
    _currentValue = _storage.readIntFromFile(path, 0); // Load initial value
    _inputManager->begin(); // Reset encoder position

    // Update the _display immediately
    _display->clear();
    _valueY = _display->drawTitle(title);
}

bool AdjustValue::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    // Handle encoder rotation
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _currentValue += (encoderDirection == InputManager::EncoderDirection::Clockwise) ? 1 : -1;
        redraw = true;
    }
    if (redraw) {
        drawScreen();
    }

    // Handle encoder button press to confirm and save
    if (_inputManager->isButtonPressed()) {
        DEBUG_PRINTLN("AdjustValue: Button pressed, saving value.");
        _storage.writeIntToFile(_path, _currentValue);
        DEBUG_PRINTLN("AdjustValue: Value saved, exiting screen.");
        return false;
    }
    return true;
}

void AdjustValue::drawScreen() {
    _display->setFont(u8g2_font_ncenB18_tn);
    char buffer[4] = {'\0'};
    sprintf(buffer, "%d", _currentValue);
    const uint8_t degreeSymbolWidth = 8;
    const uint8_t valueWidth = _display->getStrWidth(buffer);
    const uint8_t valueX = (_display->getDisplayWidth() - valueWidth - degreeSymbolWidth) / 2; // Calculate the X position to center the value
    const uint8_t valueY = _valueY + 2;

    // Only redraw the value and degree symbol
    _display->setDrawColor(0); // Clear the previous value
    const uint8_t clearWidth = valueWidth + degreeSymbolWidth + 1; // Add buffer to ensure full clearing
    const uint8_t clearHeight = _display->getAscent() - _display->getDescent() + 4; // Add buffer to ensure full clearing
    _display->drawBox(valueX, valueY - _display->getAscent(), clearWidth, clearHeight);
    _display->setDrawColor(1); // Draw the new value
    _display->drawStr(valueX, valueY, buffer);

    // Draw the custom degree symbol just after the value
    const uint8_t symbolX = valueX + valueWidth;
    const uint8_t symbolY = valueY - _display->getAscent();
    _display->drawXBMP(symbolX, symbolY, degreeSymbolWidth, degreeSymbolWidth, degreeSymbol);

    _display->sendBuffer();
}
