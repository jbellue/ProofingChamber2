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
    _display->drawButton("OK", true);
    _display->sendBuffer();
}
