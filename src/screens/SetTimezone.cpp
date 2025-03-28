#include "SetTimezone.h"
#include "icons.h"

SetTimezone::SetTimezone(DisplayManager* display, InputManager* inputManager) :
    _display(display), _inputManager(inputManager)
{}

void SetTimezone::begin() {
    beginImpl();
}

void SetTimezone::beginImpl() {
    _inputManager->begin();
    _display->clear();
    _display->drawTitle("Buy PRO to unlock", 20);
}

bool SetTimezone::update(bool forceRedraw) {
    // Handle encoder rotation
    bool redraw = forceRedraw;
    if (redraw) {
        drawScreen(); // Only redraw if necessary
    }
    return (!_inputManager->isButtonPressed());
}

void SetTimezone::drawScreen() {
    _display->setFont(u8g2_font_t0_11_tf);
    const uint8_t padding = 5;
    const char* buttonText = "OK";
    const uint8_t screenHeight = _display->getDisplayHeight();
    const uint8_t screenWidth = _display->getDisplayWidth();
    const uint8_t buttonWidth = _display->getStrWidth(buttonText);
    const uint8_t buttonX = (screenWidth - buttonWidth) / 2;
    const uint8_t buttonY = _display->getDisplayHeight() - padding;

    _display->setDrawColor(1); // Draw the new button
    _display->drawUTF8(buttonX, buttonY, buttonText);

    _display->setDrawColor(2);
    _display->drawRBox(buttonX - 5, screenHeight - 16, buttonWidth + 10, 15, 1);
    _display->sendBuffer();
}
