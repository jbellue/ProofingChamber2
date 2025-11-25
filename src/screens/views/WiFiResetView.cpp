#include "WiFiResetView.h"
#include "../../icons.h"

void WiFiResetView::showInitialPrompt() {
    if (_display) {
        clear();
        drawTitle("Effacer tous les\nparam\xC3\xA8tres du WiFi ?", 20);
    }
}

void WiFiResetView::showResetMessage() {
    if (_display) {
        clear();
        setFont(u8g2_font_t0_11_tf);
        const uint8_t titleHeight = _display->getDisplayHeight() / 2 - _display->getAscent();
        drawTitle("Reset du Wi-Fi\n" "et red\xC3\xA9marrage...", titleHeight);
        sendBuffer();
    }
}
