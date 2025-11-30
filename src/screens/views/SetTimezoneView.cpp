#include "SetTimezoneView.h"

void SetTimezoneView::showInitialPrompt() {
    if (_display) {
        clear();
        drawTitle("Buy PRO to unlock", 20);
    }
}
