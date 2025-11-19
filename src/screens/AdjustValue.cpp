#include "AdjustValue.h"
#include "DebugUtils.h"
#include "icons.h"

AdjustValue::AdjustValue(AppContext* ctx) :
    _display(ctx->display),
    _inputManager(ctx->input),
    _storage(nullptr),
    _ctx(ctx)
{}

void AdjustValue::begin(const char* title, const char* path) {
    beginImpl(title, path);
}

void AdjustValue::prepare(const char* title, const char* path) {
    _title = title;
    _path = path;
}

void AdjustValue::beginImpl() {
    beginImpl(_title, _path);
}

void AdjustValue::beginImpl(const char* title, const char* path) {
    _title = title;
    _path = path;
    // Obtain storage from the AppContext at begin time (ctx is populated in setup)
    if (_ctx && _ctx->storage) {
        _storage = _ctx->storage;
    }
    if (_storage) {
        _currentValue = _storage->readInt(path, 0); // Load initial value
    } else {
        _currentValue = 0;
    }
    _inputManager->resetEncoderPosition();

    // Update the _display immediately
    _display->clear();
    _valueY = _display->drawTitle(title);
    drawButton();
}

bool AdjustValue::update(bool shouldRedraw) {
    // Handle encoder button press to confirm and save
    if (_inputManager->isButtonPressed()) {
        DEBUG_PRINTLN("AdjustValue: Button pressed, saving value.");
        if (_storage) {
            _storage->writeInt(_path, _currentValue);
        }
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
    _display->drawButton("OK", true);
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
