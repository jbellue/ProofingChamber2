#include "AdjustTime.h"
#include "icons.h"
#include "DebugUtils.h"

AdjustTime::AdjustTime(DisplayManager* display, InputManager* inputManager) :
    _display(display),
    _inputManager(inputManager),
    _currentDays(0),
    _currentHours(0),
    _currentMinutes(0),
    _adjustingHours(true),
    _valueY(0)
{}

void AdjustTime::begin(const char* title, const uint8_t startH, const uint8_t startM) {
    DEBUG_PRINTLN("AdjustTime::begin called");
    beginImpl(title, startH, startM);
}

void AdjustTime::beginImpl(const char* title, const uint8_t startH, const uint8_t startM) {
    _title = title;
    _inputManager->begin();
    _currentHours = startH;
    _currentMinutes = startM;
    _currentDays = 0;
    _adjustingHours = true;

    // Update the display immediately
    _display->clear();
    _valueY = _display->drawTitle(title);
    drawHighlight();
    drawTime();
}

bool AdjustTime::update(bool shouldRedraw) {
    // Handle encoder rotation
    const auto encoderDirection = _inputManager->getEncoderDirection();

    if (encoderDirection != InputManager::EncoderDirection::None) {
        if (_adjustingHours) {
            _currentHours += (encoderDirection == InputManager::EncoderDirection::Clockwise) ? 1 : -1;
            if (_currentHours < 0) {
                _currentHours = 23;
                if (_currentDays > 0) _currentDays -= 1;
            }
            if (_currentHours > 23) {
                _currentHours = 0;
                _currentDays += 1;
            }
        } else {
            _currentMinutes += (encoderDirection == InputManager::EncoderDirection::Clockwise) ? 1 : -1;
            if (_currentMinutes < 0) _currentMinutes = 59;
            if (_currentMinutes > 59) _currentMinutes = 0;
        }
        drawTime(); // Redraw the time on the display
        shouldRedraw = true;
    }
    // Handle encoder button press to switch between hours and minutes, or confirm and save
    if (_inputManager->isButtonPressed()) {
        if (_adjustingHours) {
            _adjustingHours = false; // Switch to adjusting minutes
            shouldRedraw = true;
            drawHighlight();
        } else {
            DEBUG_PRINTLN("AdjustTime: Time set, exiting screen.");
            return false; // Set only when the user confirms
        }
    }
    if (shouldRedraw) {
        _display->sendBuffer(); // Send the buffer to the display
    }

    return true;
}

void AdjustTime::drawTime() {
    _display->setFont(u8g2_font_ncenB18_tn);
    char timeBuffer[6]; // Buffer for the time string (e.g., "12:34")
    sprintf(timeBuffer, "%02d:%02d", _currentHours, _currentMinutes); // Format the time as HH:MM
    const uint8_t timeWidth = _display->getStrWidth("00:00"); // Measure the width of a default time string
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2; // Calculate the X position to center the time
    const uint8_t timeY = _valueY + 2;

    // Only redraw the time string
    const uint8_t clearWidth = _display->getWidth();
    const uint8_t clearHeight = _display->getAscent();
    _display->setDrawColor(0); // Clear the previous time
    _display->drawBox(timeX, timeY - _display->getAscent(), clearWidth, clearHeight);
    _display->setDrawColor(1); // Draw the new time
    _display->drawStr(timeX, timeY, timeBuffer);

    if(_currentDays > 0) {
        _display->setFont(u8g2_font_ncenB12_tr);
        sprintf(timeBuffer, "+%dj", _currentDays); // Format the days as +1j
        _display->drawStr(timeX + timeWidth + 2, timeY - 4, timeBuffer);
    }
}

void AdjustTime::drawHighlight() {
    _display->setFont(u8g2_font_ncenB18_tn);
    const uint8_t timeWidth = _display->getStrWidth("00:00"); // Measure the width of a default time string
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2; // Calculate the X position to center the time
    const uint8_t timeY = _valueY + 2;
    const uint8_t lineLength = _display->getStrWidth("00");

    _display->setDrawColor(_adjustingHours? 0 : 1);
    _display->drawHLine(timeX + _display->getStrWidth("00:"), timeY + 2, lineLength);

    _display->setDrawColor(_adjustingHours? 1 : 0);
    _display->drawHLine(timeX, timeY + 2, lineLength);

    _display->setDrawColor(1);
}
