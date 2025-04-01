#include "AdjustTime.h"
#include "icons.h"
#include "DebugUtils.h"

AdjustTime::AdjustTime(DisplayManager* display, InputManager* inputManager) :
    _display(display), _inputManager(inputManager), 
    _selectedItem(SelectedItem::Hours), _valueY(0),
    _coolingScreen(nullptr), _menuScreen(nullptr)
{}

void AdjustTime::begin(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime) {
    DEBUG_PRINTLN("AdjustTime::begin called");
    beginImpl(title, coolingScreen, menuScreen, startTime);
}

void AdjustTime::beginImpl(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime) {
    _title = title;
    _inputManager->begin();
    _startingTime = startTime;
    _currentTime = startTime;
    _selectedItem = SelectedItem::Hours;
    _coolingScreen = coolingScreen;
    _menuScreen = menuScreen;

    // Update the display immediately
    _display->clear();
    _valueY = _display->drawTitle(title);
    drawHighlight();
    drawTime();
    drawButtons();
}

bool AdjustTime::isTimeValid(const SimpleTime& t) const {
    if (t.days > 0) return true;
    if (_startingTime.hours == 0 && _startingTime.minutes == 0) {
        return t.days >= 0 && t.hours >= 0 && t.minutes >= 0;
    }
    return t >= _startingTime;
}

SimpleTime AdjustTime::getAdjustedTime(bool isHours, bool increment) const {
    SimpleTime newTime = _currentTime;

    if (increment) {
        isHours ? newTime.incrementHours() : newTime.incrementMinutes();
    } else {
        isHours ? newTime.decrementHours() : newTime.decrementMinutes();
    }
    return newTime;
}

bool AdjustTime::update(bool shouldRedraw) {
    // Handle encoder rotation
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (encoderDirection != InputManager::EncoderDirection::None) {
        switch (_selectedItem)
        {
        case SelectedItem::Hours:
        case SelectedItem::Minutes: {
            SimpleTime newTime = getAdjustedTime(
                _selectedItem == SelectedItem::Hours,
                encoderDirection == InputManager::EncoderDirection::Clockwise
            );
            if (isTimeValid(newTime)) {
                _currentTime = newTime;
                drawTime();
            }
            break;
        }
        case SelectedItem::Ok:
            _selectedItem = SelectedItem::Cancel;
            drawButtons();
            break;
        case SelectedItem::Cancel:
            _selectedItem = SelectedItem::Ok;
            drawButtons();
            break;
        }
        shouldRedraw = true;
    }
    if (_inputManager->isButtonPressed()) {
        switch (_selectedItem)
        {
        case SelectedItem::Hours:
            _selectedItem = SelectedItem::Minutes;
            break;
        case SelectedItem::Minutes:
            _selectedItem = SelectedItem::Ok;
            drawButtons();
            break;
        case SelectedItem::Ok:
            setNextScreen(_coolingScreen);
            return false; // Exit the screen
        case SelectedItem::Cancel:
            setNextScreen(_menuScreen);
            return false; // Exit the screen
        }
        drawHighlight();
        shouldRedraw = true;
    }
    if (shouldRedraw) {
        _display->sendBuffer();
    }

    return true;
}

void AdjustTime::drawButtons() {
    _display->setFont(u8g2_font_t0_11_tf);
    const uint8_t screenHeight = _display->getDisplayHeight();
    const uint8_t screenWidth = _display->getDisplayWidth();
    const uint8_t buttonWidth = screenWidth / 2 - 1;
    const uint8_t padding = 5;
    const uint8_t buttonsY = _display->getDisplayHeight() - padding;

    // Erase the previous buttons
    _display->setDrawColor(0);
    _display->drawBox(0, screenHeight - 16, screenWidth, 16);

    _display->setDrawColor(1);
    const char* buttonText = "Démarrer";
    const uint8_t proofWidth = _display->getUTF8Width(buttonText);
    _display->drawUTF8((buttonWidth - proofWidth) / 2, buttonsY, buttonText);

    const char* cancelText = "Annuler";
    const uint8_t cancelWidth = _display->getUTF8Width(cancelText);
    _display->drawUTF8(buttonWidth + (buttonWidth - cancelWidth) / 2, buttonsY, cancelText);

    _display->setDrawColor(2);
    if (_selectedItem == SelectedItem::Ok) {
        _display->drawRBox(0, screenHeight - 16, buttonWidth, 15, 1);
    } else if (_selectedItem == SelectedItem::Cancel) {
        _display->drawRBox(buttonWidth, screenHeight - 16, buttonWidth, 15, 1);
    }
}

void AdjustTime::drawTime() {
    _display->setFont(u8g2_font_ncenB18_tf);
    char timeBuffer[6]; // Buffer for the time string (e.g., "12:34")
    sprintf(timeBuffer, "%02d:%02d", _currentTime.hours, _currentTime.minutes); // Format the time as HH:MM
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

    if(_currentTime.days > 0) {
        _display->setFont(u8g2_font_ncenB12_tr);
        sprintf(timeBuffer, "+%dj", _currentTime.days); // Format the days as +1j
        _display->drawStr(timeX + timeWidth + 2, timeY - 4, timeBuffer);
    }
}

void AdjustTime::drawHighlight() {
    _display->setFont(u8g2_font_ncenB18_tf);
    const uint8_t timeWidth = _display->getStrWidth("00:00"); // Measure the width of a default time string
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2; // Calculate the X position to center the time
    const uint8_t timeY = _valueY + 2;
    const uint8_t lineLength = _display->getStrWidth("00");

    _display->setDrawColor(0); // Clear the previous highlight
    _display->drawBox(timeX, timeY + 2, timeWidth, 2);
    if (_selectedItem == SelectedItem::Hours) {
        _display->setDrawColor(1);
        _display->drawHLine(timeX, timeY + 2, lineLength);
    } else if (_selectedItem == SelectedItem::Minutes) {
        _display->setDrawColor(1);
        _display->drawHLine(timeX + _display->getStrWidth("00:"), timeY + 2, lineLength);
    }
}
