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
    _oldPosition = _inputManager->getEncoderPosition(); // Reset encoder position
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
    const uint8_t timeWidth = _display->getStrWidth(timeBuffer);
    _display->drawUTF8((_display->getDisplayWidth() - timeWidth) / 2, 38, timeBuffer);

    const uint8_t padding = 5;
    const uint8_t screenHeight = _display->getDisplayHeight();
    const uint8_t screenWidth = _display->getDisplayWidth();
    const uint8_t buttonsY = _display->getDisplayHeight() - padding;
    const uint8_t buttonWidth = screenWidth / 2 - 1;

    // Erase the previous buttons
    _display->setDrawColor(0);
    _display->drawBox(0, screenHeight - 16, screenWidth, 16);

    _display->setDrawColor(1);
    const char* proofText = "Démarrer";
    const uint8_t proofWidth = _display->getStrWidth(proofText);
    _display->drawUTF8((buttonWidth - proofWidth) / 2, buttonsY, proofText);

    const char* cancelText = "Annuler";
    const uint8_t cancelWidth = _display->getStrWidth(cancelText);
    _display->drawUTF8(buttonWidth + (buttonWidth - cancelWidth) / 2, buttonsY, cancelText);

    _display->setDrawColor(2);
    _display->drawRBox(_onCancelButton? buttonWidth : 0, screenHeight - 16, buttonWidth, 15, 1);
    _display->sendBuffer();
}
