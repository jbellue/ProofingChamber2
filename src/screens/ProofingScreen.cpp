#include "ProofingScreen.h"
#include "icons.h"
#include "DebugUtils.h"

ProofingScreen::ProofingScreen(DisplayManager* display, InputManager* inputManager) :
    _display(display), _inputManager(inputManager), _startTime(0),
    _currentTemp(0), _isRising(false), _isIconOn(false), _previousDiffSeconds(0)
{}

void ProofingScreen::begin(tm* startTime) {
    beginImpl(startTime);
}

void ProofingScreen::beginImpl(tm* startTime) {
    _startTime = mktime((tm*)startTime);
    _currentTemp = 0;
    _isRising = true;
    _isIconOn = true;
    _previousDiffSeconds = 999999; // Force a redraw on the first update

    _display->clear();
    _display->drawTitle("En pousse depuis");
}

void ProofingScreen::drawTime() {
    // Convert to hours and minutes
    const int total_minutes = _previousDiffSeconds / 60;
    const int hours = total_minutes / 60;
    const int minutes = total_minutes % 60;

    char timeBuffer[8] = {'\0'};
    if (hours > 0) {
        snprintf(timeBuffer, sizeof(timeBuffer), "%dh%02dm", hours, minutes);
    } else {
        snprintf(timeBuffer, sizeof(timeBuffer), "%dm", minutes);
    }
    DEBUG_PRINTLN(timeBuffer);
    _display->setFont(u8g2_font_ncenB18_tf);

    // Calculate dimensions and position
    const uint8_t timeWidth = _display->getStrWidth(timeBuffer);
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2;
    const uint8_t timeY = 36;
    const uint8_t fontHeight = _display->getAscent() - _display->getDescent();

    // Clear out the previous value
    _display->setDrawColor(0); // Clear the previous value
    _display->drawBox(0, timeY - _display->getAscent() - 2, _display->getDisplayWidth(), fontHeight + 4);

    _display->setDrawColor(1);
    _display->drawUTF8(timeX, timeY, timeBuffer);
}

bool ProofingScreen::update(bool forceRedraw) {
    struct tm now;
    getLocalTime(&now);
    const time_t now_time = mktime(&now);
    const double diff_seconds = difftime(now_time, _startTime);

    if (forceRedraw || diff_seconds - _previousDiffSeconds >= 60) {
        _previousDiffSeconds = diff_seconds;
        _isRising = !_isRising;
        _isIconOn = !_isIconOn;
        drawScreen();
    }
    return !_inputManager->isButtonPressed();
}

void ProofingScreen::drawScreen() {
    drawTime();

    // Display the current temperature
    _display->setFont(u8g2_font_t0_11_tf);
    char tempBuffer[7] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%d°C", _currentTemp);
    const uint8_t tempWidth = _display->getStrWidth(tempBuffer);
    const uint8_t tempX = _display->getDisplayWidth() - tempWidth - 10;
    const uint8_t tempY = 60;

    // Clear out the previous temperature
    _display->setDrawColor(0);
    _display->drawBox(tempX - 2, tempY - _display->getAscent() - 2, tempWidth + 12, _display->getAscent() + 4);
    _display->setDrawColor(1);

    _display->drawUTF8(tempX, tempY, tempBuffer);

    const uint8_t iconsX = tempX + tempWidth;
    uint8_t iconY = tempY - _display->getAscent() * 2 - 4;
    const uint8_t proofIconSize = 10;

    // Display the small icon (on or off)
    if (_isIconOn) {
        _display->drawXBMP(iconsX, iconY, proofIconSize, proofIconSize, iconProof);
    } else {
        _display->setDrawColor(0);
        _display->drawBox(iconsX, iconY, proofIconSize, proofIconSize);
        _display->setDrawColor(1);
    }

    // Display the arrow (up or down)
    iconY += proofIconSize + 3;
    if (_isRising) {
        _display->drawXBMP(iconsX, iconY, 8, 8, iconRise);
    } else {
        _display->drawXBMP(iconsX, iconY, 8, 8, iconLower);
    }

    // Move the "Annuler" button drawing here
    const char* buttonText = "Annuler";
    const uint8_t buttonWidth = _display->getStrWidth(buttonText) + 10;
    _display->setDrawColor(1); // white button
    _display->drawRBox(5, tempY - _display->getAscent() - 3, buttonWidth, 15, 1);
    _display->setDrawColor(0); // black text
    _display->drawUTF8(10, tempY, buttonText);

    _display->sendBuffer();
}
