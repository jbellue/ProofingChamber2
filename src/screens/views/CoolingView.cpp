#include "CoolingView.h"
#include "../../DebugUtils.h"
#include "../../icons.h"

bool CoolingView::drawTime(const int remainingSeconds) {
    if(_lastRemainingSeconds == remainingSeconds) {
        return false; // No change, skip redraw
    }
    _lastRemainingSeconds = remainingSeconds;
    char timeBuffer[34] = {0};
    if (remainingSeconds < 60) {
        snprintf(timeBuffer, sizeof(timeBuffer), "dans <1m");
    } else if (remainingSeconds >= 3600) {
        snprintf(timeBuffer, sizeof(timeBuffer), "dans %dh%02dm",
            remainingSeconds / 3600, (remainingSeconds % 3600) / 60);
    } else {
        snprintf(timeBuffer, sizeof(timeBuffer), "dans %dm",
            remainingSeconds / 60);
    }
    _display->setFont(u8g2_font_t0_11_tf);

    const uint8_t timeWidth = _display->getUTF8Width(timeBuffer);
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2;
    const uint8_t timeY = 38;
    const uint8_t fontHeight = _display->getAscent() - _display->getDescent();

    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY - _display->getAscent(), timeWidth, fontHeight);

    _display->setDrawColor(1);
    _display->drawUTF8(timeX, timeY, timeBuffer);
    return true;
}

void CoolingView::drawTemperature(const float currentTemp) {
    char tempBuffer[7] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1f°", currentTemp);
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

bool CoolingView::drawIcons(bool on, bool force) {
    if (!force && _lastIconState == on) {
        return false; // No change, skip redraw
    }
    _lastIconState = on;
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
    return true;
}

void CoolingView::drawGraph(Graph& graph) {
    _display->setDrawColor(1);
    graph.draw(_display->getDisplay(), _display->getWidth() - 30, 28);
}
