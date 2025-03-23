#include "AdjustTime.h"
#include "icons.h"
#include "DebugUtils.h"

AdjustTime::AdjustTime(DisplayManager* display, InputManager* inputManager) :
    display(display),
    inputManager(inputManager),
    _oldPosition(0),
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
    _oldPosition = inputManager->getEncoderPosition(); // Reset encoder position
    _currentHours = startH;
    _currentMinutes = startM;
    _currentDays = 0;
    _adjustingHours = true;

    // Update the display immediately
    display->clear();
    _valueY = display->drawTitle(title);
}

bool AdjustTime::update(bool forceRedraw) {
    // Handle encoder rotation
    bool redraw = forceRedraw;
    const int64_t newPosition = inputManager->getEncoderPosition();
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
        redraw = true;
    }
    // Handle encoder button press to switch between hours and minutes, or confirm and save
    if (inputManager->isButtonPressed()) {
        if (_adjustingHours) {
            _adjustingHours = false; // Switch to adjusting minutes
            redraw = true;
        } else {
            DEBUG_PRINTLN("AdjustTime: Time set, exiting screen.");
            return false; // Set only when the user confirms
        }
    }
    if (redraw) {
        DEBUG_PRINTLN("AdjustTime redrawn");
        drawScreen();
    }

    return true;
}

void AdjustTime::drawScreen() {
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
