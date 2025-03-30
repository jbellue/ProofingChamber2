#include "ProofingScreen.h"
#include "icons.h"
#include "DebugUtils.h"

ProofingScreen::ProofingScreen(DisplayManager* display, InputManager* inputManager) :
    _display(display), _inputManager(inputManager), _startTime(0),
    _currentTemp(0.0), _previousTemp(0.0), _isIconOn(false), _previousDiffSeconds(0)
{}

void ProofingScreen::begin() {
    beginImpl();
}

void ProofingScreen::beginImpl() {
    struct tm startTime;
    getLocalTime(&startTime);
    _startTime = mktime(&startTime);
    _inputManager->startTemperaturePolling();
    _currentTemp = _inputManager->getTemperature();
    _isIconOn = true;
    _previousDiffSeconds = 999999; // Force a redraw on the first update
    _lastGraphUpdate = 999999;     // Force a redraw on the first update

    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
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

    _currentTemp = _inputManager->getTemperature();
    if (abs(_currentTemp - _previousTemp) > 0.1) {
        _temperatureGraph.addValueToAverage(_currentTemp);
        _previousTemp = _currentTemp;
        forceRedraw = true;
    }

    // Check if 10 seconds have elapsed since last graph update
    if (difftime(now_time, _lastGraphUpdate) >= 10.0) {
        _temperatureGraph.commitAverage(); // Commit the average value
        drawGraph();
        _lastGraphUpdate = now_time;
    }

    if (forceRedraw || diff_seconds - _previousDiffSeconds >= 60) {
        _previousDiffSeconds = diff_seconds;
        _isIconOn = !_isIconOn;
        drawScreen();
    }
    if (_inputManager->isButtonPressed()) {
        _inputManager->stopTemperaturePolling();
        return false; // Return to the previous screen
    }
    return true;
}

void ProofingScreen::drawGraph() {
    _display->setDrawColor(1);
    _temperatureGraph.draw(_display->getDisplay(), _display->getWidth() - 30, 48);

    _display->sendBuffer();
}


void ProofingScreen::drawScreen() {
    drawTime();

    // Display the current temperature
    _display->setFont(u8g2_font_t0_11_tf);
    char tempBuffer[7] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1f°C", _currentTemp);
    const uint8_t tempWidth = _display->getStrWidth(tempBuffer);
    const uint8_t tempX = _display->getDisplayWidth() / 2;
    const uint8_t tempY = 60;

    // Clear out the previous temperature
    _display->setDrawColor(0);
    _display->drawBox(tempX - 2, tempY - _display->getAscent() - 2, tempWidth, _display->getAscent() + 4);
    _display->setDrawColor(1);

    _display->drawUTF8(tempX, tempY, tempBuffer);

    const uint8_t proofIconSize = 10;
    const uint8_t iconsX = _display->getDisplayWidth() - proofIconSize - 2;
    const uint8_t iconY = 35;

    // Display the small icon (on or off)
    if (_isIconOn) {
        _display->drawXBMP(iconsX, iconY, proofIconSize, proofIconSize, iconProof);
    } else {
        _display->setDrawColor(0);
        _display->drawBox(iconsX, iconY, proofIconSize, proofIconSize);
        _display->setDrawColor(1);
    }

    const char* buttonText = "Annuler";
    const uint8_t buttonWidth = _display->getStrWidth(buttonText) + 10;
    _display->setDrawColor(1); // white button
    _display->drawRBox(5, tempY - _display->getAscent() - 3, buttonWidth, 15, 1);
    _display->setDrawColor(0); // black text
    _display->drawUTF8(10, tempY, buttonText);

    _display->sendBuffer();
}
