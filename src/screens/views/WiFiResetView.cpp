#include "WiFiResetView.h"
#include "../../icons.h"

void WiFiResetView::showInitialPrompt() {
    if (_display) {
        _display->clear();
        _display->drawTitle("Effacer tous les\nparam\xC3\xA8tres du WiFi ?", 20);
    }
}

void WiFiResetView::showResetMessage() {
    if (_display) {
        _display->clear();
        _display->setFont(u8g2_font_t0_11_tf);
        const uint8_t titleHeight = _display->getDisplayHeight() / 2 - _display->getAscent();
        _display->drawTitle("Reset du Wi-Fi\n" "et red\xC3\xA9marrage...", titleHeight);
        _display->sendBuffer();
    }
}

void WiFiResetView::drawButtons(bool onCancelButton) {
    if (_display) {
        _display->drawButtons("Confirmer", "Annuler", onCancelButton ? 1 : 0);
        _display->sendBuffer();
    }
}
