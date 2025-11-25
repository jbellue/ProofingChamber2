#include "CoolingView.h"


void CoolingView::drawTime(const char* timeBuffer) {
    if (_display) {
        uint8_t timeWidth = _display->getUTF8Width(timeBuffer);
        _display->drawUTF8((_display->getDisplayWidth() - timeWidth) / 2, 38, timeBuffer);
    }
}

