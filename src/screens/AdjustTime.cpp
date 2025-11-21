#include "AdjustTime.h"
#include "icons.h"
#include "DebugUtils.h"

AdjustTime::AdjustTime(AppContext* ctx) :
    _display(nullptr), _inputManager(nullptr),
    _selectedItem(SelectedItem::Hours), _valueY(0),
    _coolingScreen(nullptr), _menuScreen(nullptr), _ctx(ctx)
{}

void AdjustTime::begin(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime) {
    DEBUG_PRINTLN("AdjustTime::begin called");
    beginImpl(title, coolingScreen, menuScreen, startTime);
}

void AdjustTime::beginImpl(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime) {
    _title = title;
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_display) _display = _ctx->display;
    }
    if (_inputManager) _inputManager->resetEncoderPosition();
    _startingTime = startTime;
    _currentTime = startTime;
    _selectedItem = SelectedItem::Hours;
    _coolingScreen = coolingScreen;
    _menuScreen = menuScreen;

    // Update the display immediately
    if (_display) {
        _display->clear();
        _valueY = _display->drawTitle(title);
        drawHighlight();
        drawTime();
        drawButtons();
    }
}

void AdjustTime::prepare(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime) {
    _title = title;
    _coolingScreen = coolingScreen;
    _coolingScreen = coolingScreen;
    _menuScreen = menuScreen;
    _startingTime = startTime;
}

void AdjustTime::beginImpl() {
    beginImpl(_title, _coolingScreen, _menuScreen, _startingTime);
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
    if (_inputManager && _inputManager->isButtonPressed()) {
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
    if (shouldRedraw && _display) {
        _display->sendBuffer();
    }

    return true;
}

void AdjustTime::drawButtons() {
    int8_t highlightedButton = -1;
    if (_selectedItem == SelectedItem::Ok) {
        highlightedButton = 0; // Highlight the "Démarrer" button
    } else if (_selectedItem == SelectedItem::Cancel) {
        highlightedButton = 1; // Highlight the "Annuler" button
    }
    _display->drawButtons("Démarrer", "Annuler", highlightedButton);
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
