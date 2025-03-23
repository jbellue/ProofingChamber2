#include "ProofingScreen.h"
#include "icons.h"
#include "DebugUtils.h"

ProofingScreen::ProofingScreen(DisplayManager* display, InputManager* inputManager) :
    display(display), inputManager(inputManager), startTime(nullptr),
    currentTemp(0), isRising(false), isIconOn(false)
{}

void ProofingScreen::begin(const char* startTime, int initialTemp, bool isRising, bool isIconOn) {
    beginImpl(startTime, initialTemp, isRising, isIconOn);
}

void ProofingScreen::beginImpl(const char* startTime, int initialTemp, bool isRising, bool isIconOn) {
    this->startTime = startTime;
    this->currentTemp = initialTemp;
    this->isRising = isRising;
    this->isIconOn = isIconOn;

    display->clear();
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        DEBUG_PRINTLN("Failed to obtain time");
        display->drawTitle("En pousse\n");
    } else {
        char timeBuffer[32] = {'\0'};
        snprintf(timeBuffer, sizeof(timeBuffer), "En pousse depuis\n%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        DEBUG_PRINTLN(timeBuffer);
        display->drawTitle(timeBuffer);
    }
}

bool ProofingScreen::update(bool forceRedraw) {
    if (forceRedraw) {
        drawScreen();
    }
    return !inputManager->isButtonPressed();
}


void ProofingScreen::drawScreen() {
    // Display the current temperature
    display->setFont(u8g2_font_ncenB18_tf);
    char tempBuffer[8] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%d°C", currentTemp);
    display->drawUTF8(40, 45, tempBuffer);

    // Display the arrow (up or down)
    if (isRising) {
        display->drawUTF8(80, 45, "↑");
    } else {
        display->drawUTF8(80, 45, "↓");
    }

    // Display the "Annuler" button
    display->setFont(u8g2_font_t0_11_tf);
    display->drawUTF8(10, 60, "Annuler");

    // Display the small icon (on or off)
    if (isIconOn) {
        display->drawXBMP(100, 10, 10, 10, iconProof);
    }

    display->sendBuffer();
}
