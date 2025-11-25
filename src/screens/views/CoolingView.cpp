#include "CoolingView.h"

CoolingView::CoolingView(DisplayManager* display)
    : _display(display) {}

CoolingView::~CoolingView() {}

void CoolingView::clear() {
    if (_display) _display->clearBuffer();
}

void CoolingView::drawTitle(const char* title) {
    if (_display) _display->drawTitle(title);
}

void CoolingView::drawButtons(const char* startLabel, const char* cancelLabel, int selectedButton) {
    if (_display) _display->drawButtons(startLabel, cancelLabel, selectedButton);
}

void CoolingView::drawTime(const char* timeBuffer) {
    if (_display) {
        uint8_t timeWidth = _display->getUTF8Width(timeBuffer);
        _display->drawUTF8((_display->getDisplayWidth() - timeWidth) / 2, 38, timeBuffer);
    }
}

void CoolingView::sendBuffer() {
    if (_display) _display->sendBuffer();
}

int CoolingView::getDisplayWidth() const {
    return _display ? _display->getDisplayWidth() : 0;
}

uint8_t CoolingView::getUTF8Width(const char* str) const {
    return _display ? _display->getUTF8Width(str) : 0;
}
