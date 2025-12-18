#include "ConfirmTimezoneView.h"

void ConfirmTimezoneView::setTimezone(const char* timezoneName) {
    _timezoneName = timezoneName;
}

void ConfirmTimezoneView::start() {
    clear();
    if (_timezoneName) {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "Enregistrer le fuseau\n%s ?", _timezoneName);
        drawTitle(buffer, 20);
    } else {
        drawTitle("Enregistrer le fuseau ?", 20);
    }
    drawButtons(true);
}

void ConfirmTimezoneView::drawButtons(bool onCancelSelected) {
    const char* buttons[] = {"OK", "Annuler"};
    IBaseView::drawButtons(buttons, 2, onCancelSelected ? 1 : 0);
}
