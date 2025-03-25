#include "CoolingScreen.h"
#include "icons.h"
#include "DebugUtils.h"

CoolingScreen::CoolingScreen(DisplayManager* display, InputManager* inputManager)
    : _display(display), _inputManager(inputManager), _endTime(0), _proofingScreen(nullptr), _menuScreen(nullptr) {}

void CoolingScreen::begin(time_t endTime, Screen* proofingScreen, Screen* menuScreen) {
    beginImpl(endTime, proofingScreen, menuScreen);
}
void CoolingScreen::beginImpl(time_t endTime, Screen* proofingScreen, Screen* menuScreen) {
    _endTime = endTime;
    _proofingScreen = proofingScreen;
    _menuScreen = menuScreen;
    _onCancelButton = false;
    _oldPosition = _inputManager->getEncoderPosition(); // Reset encoder position

    _display->clear();
    _display->drawTitle("Cooling in progress");
}

bool CoolingScreen::update(bool forceRedraw) {
    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);

    bool isButtonPressed = _inputManager->isButtonPressed();
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

    bool redraw = forceRedraw;
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
    time_t now = time(nullptr);
    int remainingSeconds = static_cast<int>(_endTime - now);
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
