#include "WiFiReset.h"
#include "icons.h"
// Include concrete service headers so we can call their methods
#include "../services/INetworkService.h"
#include "../services/IRebootService.h"

WiFiReset::WiFiReset(AppContext* ctx) :
    _display(ctx->display), _inputManager(ctx->input), _networkService(ctx->networkService), _reboot_service(ctx->rebootService), _ctx(ctx), _onCancelButton(true)
{}

void WiFiReset::begin() {
    beginImpl();
}

void WiFiReset::beginImpl() {
    _onCancelButton = true;
    _inputManager->resetEncoderPosition();
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
        // Reset Wi-Fi credentials via network service
        if (_networkService) {
            _networkService->resetSettings();
        }

        // Notify the user
        _display->clear(); // Clear the screen for the reset message
        _display->setFont(u8g2_font_t0_11_tf);
        const uint8_t titleHeight = _display->getDisplayHeight() / 2 - _display->getAscent();
        _display->drawTitle("Reset du Wi-Fi\n" "et red\xC3\xA9marrage...", titleHeight);
        _display->sendBuffer();

        // Delay to allow the message to be displayed
        delay(2000);

        // Reboot the device via the injected reboot service
        if (_reboot_service) {
            _reboot_service->reboot();
        }
        delay(2000);  // delay just to keep that screen on so nothing blinks before reboot
    }
    return true;
}


void WiFiReset::drawScreen() {
    _display->drawButtons("Confirmer", "Annuler", _onCancelButton ? 1 : 0);
    _display->sendBuffer();
}
