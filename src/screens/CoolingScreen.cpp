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
    _onCancelButton = true;
    _inputManager->begin();
    _endTime = 0;
    _lastUpdateTime = 0;

    _display->clear();
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

    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        redraw = true;
    }

    if (redraw) {
        drawScreen();
    }
    return true; // Stay on the current screen
}

void CoolingScreen::drawScreen() {
    _display->clearBuffer();
    const tm* tm_end = localtime(&_endTime);
    char timeBuffer[34] = {'\0'};
    snprintf(timeBuffer, sizeof(timeBuffer), "Démarrage de la\npousse \xC3\xA0 %d:%02d", tm_end->tm_hour, tm_end->tm_min);
    _display->drawTitle(timeBuffer);
    _display->setFont(u8g2_font_t0_11_tf);

    // Display remaining time
    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);
    const int remainingSeconds = difftime(_endTime, now);
    if (remainingSeconds >= 3600) {
        snprintf(timeBuffer, sizeof(timeBuffer), "(dans %dh%02dm)",
            remainingSeconds / 3600, (remainingSeconds % 3600) / 60);
    } else if (remainingSeconds >= 60) {
        snprintf(timeBuffer, sizeof(timeBuffer), "(dans %dm%02ds)",
            remainingSeconds / 60, remainingSeconds % 60);
    } else {
        snprintf(timeBuffer, sizeof(timeBuffer), "(dans %ds)", remainingSeconds);
    }
    const uint8_t timeWidth = _display->getUTF8Width(timeBuffer);
    _display->drawUTF8((_display->getDisplayWidth() - timeWidth) / 2, 38, timeBuffer);

    _display->drawButtons("Démarrer", "Annuler", _onCancelButton ? 1 : 0);
    _display->sendBuffer();
}
