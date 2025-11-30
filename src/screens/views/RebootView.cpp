#include "RebootView.h"

void RebootView::start() {
    clear();
    drawTitle("Red\xC3\xA9marrer ?", 20);
    sendBuffer();
}
