#include "RebootView.h"
#include "../../icons.h"

RebootView::RebootView(DisplayManager* display) : _display(display) {}

void RebootView::showTitle() {
    _display->clear();
    _display->drawTitle("Red\xC3\xA9marrer ?", 20);
}

void RebootView::drawButtons(bool onCancel) {
    _display->drawButtons("Confirmer", "Annuler", onCancel ? 1 : 0);
    _display->sendBuffer();
}
