#include "PowerOffView.h"

void PowerOffView::start() {
    clear();
    drawTitle("\xC3\x89teindre ?", 20);
    drawButtons(true);
}

void PowerOffView::drawButtons(bool onCancelSelected) {
    const char* buttons[] = {"Confirmer", "Annuler"};
    IBaseView::drawButtons(buttons, 2, onCancelSelected ? 1 : 0);
}
