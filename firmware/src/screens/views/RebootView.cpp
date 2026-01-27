#include "RebootView.h"

void RebootView::start() {
    clear();
    drawTitle("Red\xC3\xA9marrer ?", 20);
    drawButtons(true);
}

void RebootView::drawButtons(bool onCancelSelected) {
    const char* buttons[] = {"Confirmer", "Annuler"};
    IBaseView::drawButtons(buttons, 2, onCancelSelected ? 1 : 0);
}
