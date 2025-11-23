#include "RebootView.h"
#include "../../icons.h"

void RebootView::showTitle() {
    clear();
    _display->drawTitle("Red\xC3\xA9marrer ?", 20);
}

void RebootView::drawButtons(bool onCancel) {
    _display->drawButtons("Confirmer", "Annuler", onCancel ? 1 : 0);
    sendBuffer();
}
