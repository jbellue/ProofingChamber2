#include <WiFiManager.h>
#include "WiFiReset.h"
#include "icons.h"

WiFiReset::WiFiReset(DisplayManager* display, InputManager* inputManager) :
    _display(display), _inputManager(inputManager), _onCancelButton(true)
{}

void WiFiReset::begin() {
    beginImpl();
}

void WiFiReset::beginImpl() {
    _onCancelButton = true;
    _inputManager->begin();
    _display->clear();
    _display->drawTitle("Effacer tous les\nparam\xC3\xA8tres du WiFi ?", 20);
}

bool WiFiReset::update(bool forceRedraw) {
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
        // Reset Wi-Fi credentials
        WiFiManager wifiManager;
        wifiManager.resetSettings();

        // Notify the user
        _display->clear(); // Clear the screen for the reset message
        _display->setFont(u8g2_font_t0_11_tf);
        const uint8_t titleHeight = _display->getDisplayHeight() / 2 - _display->getAscent();
        _display->drawTitle("Reset du Wi-Fi\n" "et red\xC3\xA9marrage...", titleHeight);
        _display->sendBuffer();

        // Delay to allow the message to be displayed
        delay(2000);

        // Reboot the device
        ESP.restart();
        delay(2000);  // delay just to keep that screen on so nothing blinks before reboot
    }
    return true;
}


void WiFiReset::drawScreen() {
    _display->setFont(u8g2_font_t0_11_tf);
    const uint8_t padding = 5;
    const uint8_t screenHeight = _display->getDisplayHeight();
    const uint8_t screenWidth = _display->getDisplayWidth();
    const uint8_t buttonsY = _display->getDisplayHeight() - padding;
    const uint8_t buttonWidth = screenWidth / 2 - 1;

    // Erase the previous buttons
    _display->setDrawColor(0); // Clear the previous time
    _display->drawBox(0, screenHeight - 20, screenWidth, 20);

    _display->setDrawColor(1); // Draw the new buttons
    const char* confirmText = "Confirmer";
    const uint8_t confirmWidth = _display->getStrWidth(confirmText);
    _display->drawUTF8((buttonWidth - confirmWidth) / 2, buttonsY, confirmText);
    const char* cancelText = "Annuler";
    const uint8_t cancelWidth = _display->getStrWidth(cancelText);
    _display->drawUTF8(buttonWidth + (buttonWidth - cancelWidth) / 2, buttonsY, cancelText);

    _display->setDrawColor(2);
    _display->drawRBox(_onCancelButton? buttonWidth : 0, screenHeight - 16, buttonWidth, 15, 1);
    _display->sendBuffer();
}
