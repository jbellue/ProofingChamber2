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
    drawButton();
}

bool AdjustValue::update(bool shouldRedraw) {
    // Handle encoder button press to confirm and save
    if (_inputManager->isButtonPressed()) {
        DEBUG_PRINTLN("AdjustValue: Button pressed, saving value.");
        _storage.writeIntToFile(_path, _currentValue);
        DEBUG_PRINTLN("AdjustValue: Value saved, exiting screen.");
        return false;
    }
    // Handle encoder rotation
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (shouldRedraw || encoderDirection != InputManager::EncoderDirection::None) {
        if (encoderDirection == InputManager::EncoderDirection::Clockwise) {
            _currentValue += 1;
        } else if (encoderDirection == InputManager::EncoderDirection::CounterClockwise) {
            _currentValue -= 1;
        }
        drawValue(); // Redraw the value on the display
        shouldRedraw = true;
    }
    if (shouldRedraw) {
        _display->sendBuffer(); // Send the buffer to the display
    }
    return true;
}

void AdjustValue::drawButton() {
    _display->setFont(u8g2_font_t0_11_tf);
    const uint8_t padding = 2;
    const char* buttonText = "OK";
    const uint8_t screenHeight = _display->getDisplayHeight();
    const uint8_t screenWidth = _display->getDisplayWidth();
    const uint8_t buttonWidth = _display->getStrWidth(buttonText);
    const uint8_t buttonX = (screenWidth - buttonWidth) / 2;
    const uint8_t buttonY = screenHeight - padding;

    _display->setDrawColor(1); // Draw the new button
    _display->drawUTF8(buttonX, buttonY, buttonText);

    _display->setDrawColor(2);
    _display->drawRBox(buttonX - 7, screenHeight - 12, buttonWidth + 14, 12, 1);
}

void AdjustValue::drawValue() {
    _display->setFont(u8g2_font_ncenB18_tf);
    char buffer[6] = {'\0'}; // "999°" + '\0' ; degree symbol is two bytes
    const uint8_t writtenChars = sprintf(buffer, "%d", _currentValue);
    const uint8_t valueWidth = _display->getUTF8Width(buffer);
    buffer[writtenChars] = 0xC2;    // Add the degree symbol after measurement
    buffer[writtenChars+1] = 0xB0;  // as the multibyte character it is
    const uint8_t valueX = (_display->getDisplayWidth() - valueWidth) / 2; // Calculate the X position to center the value
    const uint8_t valueY = _valueY + 1;

    _display->setDrawColor(0); // Clear the previous value
    const uint8_t ascent = _display->getAscent();
    _display->drawBox(0, valueY - ascent, _display->getDisplayWidth(), ascent);
    _display->setDrawColor(1); // Draw the new value
    _display->drawStr(valueX, valueY, buffer);
}
