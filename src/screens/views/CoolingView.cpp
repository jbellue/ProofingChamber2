#include "CoolingView.h"
#include "../../DebugUtils.h"
#include "../../icons.h"


void CoolingView::drawTime(const char* timeBuffer) {
    if (_display) {
        uint8_t timeWidth = _display->getUTF8Width(timeBuffer);
        _display->drawUTF8((_display->getDisplayWidth() - timeWidth) / 2, 38, timeBuffer);
    }
}

void CoolingView::drawTemperature(const char* tempBuffer) {
    _display->setFont(u8g2_font_t0_11_tf);
    const uint8_t tempWidth = _display->getUTF8Width("99.9°");
    const uint8_t tempHeight = _display->getAscent() - _display->getDescent();
    const uint8_t tempX = _display->getDisplayWidth() - tempWidth;
    const uint8_t tempY = 24;

    _display->setDrawColor(0);
    _display->drawBox(tempX, tempY - _display->getAscent(), tempWidth, tempHeight);
    _display->setDrawColor(1);

    _display->drawUTF8(tempX, tempY, tempBuffer);
}

void CoolingView::drawIcons(bool on) {
    const uint8_t coolIconSize = 10;
    const uint8_t iconsX = 2;
    const uint8_t iconY = 33;
    if (on) {
        _display->drawXBMP(iconsX, iconY, coolIconSize, coolIconSize, iconCool);
    } else {
        _display->setDrawColor(0);
        _display->drawBox(iconsX, iconY, coolIconSize, coolIconSize);
        _display->setDrawColor(1);
    }
}

void CoolingView::drawGraph(Graph& graph) {
    _display->setDrawColor(1);
    graph.draw(_display->getDisplay(), _display->getWidth() - 30, 28);
}
