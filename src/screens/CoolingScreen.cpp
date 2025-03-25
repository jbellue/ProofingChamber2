#include "CoolingScreen.h"
#include "icons.h"
#include "DebugUtils.h"

CoolingScreen::CoolingScreen(DisplayManager* display, InputManager* inputManager)
    : _display(display), _inputManager(inputManager), _endTime(0), _proofingScreen(nullptr),
    _menuScreen(nullptr), _timeCalculator(nullptr), _lastUpdateTime(0) {}

void CoolingScreen::begin(TimeCalculatorCallback callback, Screen* proofingScreen, Screen* menuScreen) {
    beginImpl(callback, proofingScreen, menuScreen);
}
void CoolingScreen::beginImpl(TimeCalculatorCallback callback, Screen* proofingScreen, Screen* menuScreen) {
    _timeCalculator = callback;
    _proofingScreen = proofingScreen;
    _menuScreen = menuScreen;
    _onCancelButton = false;
    _oldPosition = _inputManager->getEncoderPosition(); // Reset encoder position
    _endTime = 0;
    _lastUpdateTime = 0;

    _display->clear();
    _display->drawTitle("Cooling in progress");
}

bool CoolingScreen::update(bool forceRedraw) {
    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);

    bool isButtonPressed = _inputManager->isButtonPressed();
    if(forceRedraw) {
        _endTime = _timeCalculator();
    }
    if (isButtonPressed && !_onCancelButton || now >= _endTime) {
        setNextScreen(_proofingScreen);
        _proofingScreen->setNextScreen(_menuScreen);
        _proofingScreen->begin();
        return false;
    }
    if (isButtonPressed && _onCancelButton) {
        setNextScreen(_menuScreen);
        _menuScreen->begin();
        return false;
    }

    bool redraw = forceRedraw || (now != _lastUpdateTime);
    _lastUpdateTime = now;

    const int64_t newPosition = _inputManager->getEncoderPosition();
    if (newPosition != _oldPosition) {
        _onCancelButton = !_onCancelButton;
        _oldPosition = newPosition;
        redraw = true;
    }

    if (redraw) {
        drawScreen();
    }
    return true; // Stay on the current screen
}

void CoolingScreen::drawScreen() {
    _display->clearBuffer();
    _display->setFont(u8g2_font_t0_11_tf);

    // Display title
    _display->drawUTF8(10, 10, "Cooling in progress");

    // Display remaining time
    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);
    const int remainingSeconds = difftime(_endTime, now);
    char timeBuffer[16];
    snprintf(timeBuffer, sizeof(timeBuffer), "Ends in %02d:%02d", remainingSeconds / 60, remainingSeconds % 60);
    _display->drawUTF8(10, 30, timeBuffer);

    _display->setDrawColor(1);
    _display->drawUTF8(20, 60, "Annuler");
    _display->drawUTF8(90, 60, "Pousser");

    _display->setDrawColor(2);
    _display->drawRBox(_onCancelButton ? 10 : 80, 50, 60, 15, 2);

    _display->sendBuffer();
}
