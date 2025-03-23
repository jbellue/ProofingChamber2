#include "ProofingScreen.h"
#include "icons.h"

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
}

bool ProofingScreen::update(bool forceRedraw) {
    if (forceRedraw) {
        drawScreen();
    }
    return !inputManager->isButtonPressed();
}


void ProofingScreen::drawScreen() {
    display->clearBuffer();

    // Display the title "En pousse depuis <startTime>"
    display->setFont(u8g2_font_t0_11_tf);
    display->drawUTF8(10, 10, "En pousse depuis");
    display->drawUTF8(10, 20, startTime);

    // Display the current temperature
    display->setFont(u8g2_font_ncenB18_tn);
    char tempBuffer[8];
    sprintf(tempBuffer, "%d°C", currentTemp);
    display->drawUTF8(40, 40, tempBuffer);

    // Display the arrow (up or down)
    if (isRising) {
        display->drawUTF8(80, 40, "↑");
    } else {
        display->drawUTF8(80, 40, "↓");
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
