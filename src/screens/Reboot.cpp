#include "Reboot.h"
#include "icons.h"

Reboot::Reboot(DisplayManager* display, InputManager* inputManager) :
    _display(display), _inputManager(inputManager), _onCancelButton(true)
{}

void Reboot::begin() {
    beginImpl();
}

void Reboot::beginImpl() {
    _onCancelButton = true;
    _inputManager->resetEncoderPosition();
    _display->clear();
    _display->drawTitle("Red\xC3\xA9marrer ?", 20);
}

bool Reboot::update(bool forceRedraw) {
    // Handle encoder rotation
    bool redraw = forceRedraw;
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        redraw = true;
    }
    if (redraw) {
        drawScreen(); // Only redraw if necessary
    }
    if (_inputManager->isButtonPressed()) {
        if (_onCancelButton) {
            return false;
        }
        // Reboot the device
        ESP.restart();
        delay(2000);  // delay just to keep that screen on so nothing blinks before reboot
    }
    return true;
}


void Reboot::drawScreen() {
    _display->drawButtons("Confirmer", "Annuler", _onCancelButton ? 1 : 0);
    _display->sendBuffer();
}
