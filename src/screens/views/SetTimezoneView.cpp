#include "SetTimezoneView.h"


void SetTimezoneView::start() {
    clear();
    drawTitle("Buy PRO to unlock", 20);
    const char* buttons[] = {"OK"};
    drawButtons(buttons, 1, 0);
    sendBuffer();
}
