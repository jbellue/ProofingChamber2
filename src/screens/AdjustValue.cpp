#include "AdjustValue.h"
#include "DebugUtils.h"
#include "icons.h"

AdjustValue::AdjustValue(DisplayManager* display, InputManager* inputManager) :
    display(display),
    inputManager(inputManager),
    currentTemp(0),
    isRising(true),
    isIconOn(true)
{}

void AdjustValue::begin(const char* title, const char* path) {
    beginImpl(title, path);
}

void AdjustValue::beginImpl(const char* title, const char* path) {
    _title = title;
    _path = path;
    _currentValue = _storage.readIntFromFile(path, 0); // Load initial value
    _oldPosition = inputManager->getEncoderPosition(); // Reset encoder position

    // Update the display immediately
    display->clear();
    _valueY = display->drawTitle(title);
}

bool AdjustValue::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    // Handle encoder rotation
    int64_t newPosition = inputManager->getEncoderPosition();
    if (newPosition != _oldPosition) {
        _currentValue += (newPosition > _oldPosition) ? 1 : -1;
        _oldPosition = newPosition;
        redraw = true;
    }
    if (redraw) {
        drawScreen();
    }

    // Handle encoder button press to confirm and save
    if (inputManager->isButtonPressed()) {
        DEBUG_PRINTLN("AdjustValue: Button pressed, saving value.");
        _storage.writeIntToFile(_path, _currentValue);
        DEBUG_PRINTLN("AdjustValue: Value saved, exiting screen.");
        return false;
    }
    return true;
}

void AdjustValue::drawScreen() {
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
