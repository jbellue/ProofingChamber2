#include "SetTimezoneView.h"
#include "../../icons.h"

void SetTimezoneView::showInitialPrompt() {
    if (_display) {
        _display->clear();
        _display->drawTitle("Buy PRO to unlock", 20);
    }
}

void SetTimezoneView::drawButton() {
    if (_display) {
        _display->drawButton("OK", true);
        _display->sendBuffer();
    }
}
