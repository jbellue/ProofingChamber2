#include "CoolingScreen.h"
#include "icons.h"
#include "DebugUtils.h"

CoolingScreen::CoolingScreen(AppContext* ctx)
    : _display(nullptr), _inputManager(nullptr), _endTime(0), _ProofingController(nullptr),
    _menuScreen(nullptr), _timeCalculator(nullptr), _lastUpdateTime(0), _temperatureController(nullptr), _ctx(ctx) {}

void CoolingScreen::begin(TimeCalculatorCallback callback, Screen* ProofingController, Screen* menuScreen) {
    beginImpl(callback, ProofingController, menuScreen);
}
void CoolingScreen::beginImpl(TimeCalculatorCallback callback, Screen* ProofingController, Screen* menuScreen) {
    // Late-bind context pointers
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_display) _display = _ctx->display;
        if (!_temperatureController) _temperatureController = _ctx->tempController;
    }
    _timeCalculator = callback;
    _ProofingController = ProofingController;
    _menuScreen = menuScreen;
    _onCancelButton = true;
    if (_inputManager) {
        _inputManager->resetEncoderPosition();
        _inputManager->slowTemperaturePolling(false);
    }
    _endTime = 0;
    _lastUpdateTime = 0;

    if (_temperatureController) _temperatureController->setMode(TemperatureController::COOLING);

    if (_display) _display->clear();
}

bool CoolingScreen::update(bool shouldRedraw) {
    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);

    bool isButtonPressed = _inputManager ? _inputManager->isButtonPressed() : false;
    if(shouldRedraw) {
        _endTime = _timeCalculator();
    }
    bool timesUp = now >= _endTime;
    if(isButtonPressed || timesUp) {
        if (_inputManager) _inputManager->slowTemperaturePolling(true);
        if (_temperatureController) _temperatureController->setMode(TemperatureController::OFF);

        bool goingToProofScreen = !_onCancelButton || timesUp;
        Screen* nextScreen = goingToProofScreen ? _ProofingController : _menuScreen;
        setNextScreen(nextScreen);
        if (goingToProofScreen) {
            _ProofingController->setNextScreen(_menuScreen);
        }
        if (nextScreen) nextScreen->begin();
        return false;
    }
    if (difftime(now, _lastUpdateTime) >= 1) {
        const float currentTemp = _inputManager ? _inputManager->getTemperature() : 0.0f;
        DEBUG_PRINT("Current temperature: ");
        DEBUG_PRINT(currentTemp);
        DEBUG_PRINT(" - Previous: ");
        DEBUG_PRINTLN(_previousTemp);
        _lastUpdateTime = now;
        if (abs(currentTemp - _previousTemp) > 0.1) {
            _previousTemp = currentTemp;
            if (_temperatureController) _temperatureController->update(currentTemp);
        }
        shouldRedraw = true;
    }

    const auto encoderDirection = _inputManager ? _inputManager->getEncoderDirection() : InputManager::EncoderDirection::None;
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        shouldRedraw = true;
    }

    if (shouldRedraw) {
        drawScreen();
    }
    return true; // Stay on the current screen
}

void CoolingScreen::drawScreen() {
    if (!_display) return;
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

void CoolingScreen::prepare(TimeCalculatorCallback callback, Screen* ProofingController, Screen* menuScreen) {
    _timeCalculator = callback;
    _ProofingController = ProofingController;
    _menuScreen = menuScreen;
}

void CoolingScreen::beginImpl() {
    // If parameters were prepared earlier, call the param overload to initialize
    if (_timeCalculator) {
        beginImpl(_timeCalculator, _ProofingController, _menuScreen);
    } else {
        // No parameters prepared — fallback to default initialization
        beginImpl(_timeCalculator, _ProofingController, _menuScreen);
    }
}
