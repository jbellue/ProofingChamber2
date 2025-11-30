#include "CoolingView.h"
#include "../../DebugUtils.h"

#define GRAPH_POSITION_FROM_LEFT 30

void CoolingView::start(const time_t endTime, bool onCancelSelected, Graph& graph) {
    reset();
    clear();
    drawTitle(endTime);
    drawButtons(onCancelSelected);
    drawGraph(graph);
}

bool CoolingView::drawTime(const int remainingSeconds) {
    if(_lastRemainingSeconds == remainingSeconds) {
        return false; // No change, skip redraw
    }
    _lastRemainingSeconds = remainingSeconds;
    char timeBuffer[34] = {0}; // "(dans 999h59m)"
    formatTimeString(timeBuffer, sizeof(timeBuffer), remainingSeconds);
    _display->setFont(u8g2_font_t0_11_tf);

    const uint8_t timeX = 2;
    const uint8_t timeY = 42;
    const uint8_t fontHeight = _display->getAscent() - _display->getDescent();

    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY - _display->getAscent(), _timeWidth, fontHeight);

    _timeWidth = _display->getUTF8Width(timeBuffer);

    _display->setDrawColor(1);
    _display->drawUTF8(timeX, timeY, timeBuffer);
    return true;
}

bool CoolingView::drawTemperature(const float currentTemp) {

    if (abs(_lastTemperature - currentTemp) < 0.1) {
        return false; // No significant change, skip redraw
    }
    _lastTemperature = currentTemp;
    char tempBuffer[7] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1f°", currentTemp);
    _display->setFont(u8g2_font_t0_11_tf);
    const uint8_t tempWidth = _display->getUTF8Width("99.9°");
    const uint8_t tempHeight = _display->getAscent() - _display->getDescent();
    const uint8_t tempX = _display->getDisplayWidth() - tempWidth;
    const uint8_t tempY = 32;

    _display->setDrawColor(0);
    _display->drawBox(tempX, tempY - _display->getAscent(), tempWidth, tempHeight);
    _display->setDrawColor(1);

    _display->drawUTF8(tempX, tempY, tempBuffer);
    return true;
}

bool CoolingView::drawIcons(IconState iconState) {
    if (iconState == _lastIconState) {
        return false; // No change, skip redraw
    }
    _lastIconState = iconState;
    const uint8_t coolIconSize = 10;
    const uint8_t iconsX = _display->getDisplayWidth() - GRAPH_POSITION_FROM_LEFT - coolIconSize - 2;
    const uint8_t iconY = 36;
    if (iconState == IconState::On) {
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
    graph.draw(_display->getDisplay(), _display->getWidth() - GRAPH_POSITION_FROM_LEFT, 34);
}

void CoolingView::formatTimeString(char* buffer, const size_t bufferSize, const int remainingSeconds) {
    const char* prefix = "(dans ";
    const char* suffix = ")";
    if (remainingSeconds < 60) {
        snprintf(buffer, bufferSize, "%s<1m%s", prefix, suffix);
    } else if (remainingSeconds > 3596400) {
        snprintf(buffer, bufferSize, "%slongtemps%s", prefix, suffix);
    } else if (remainingSeconds >= 3600) {
        snprintf(buffer, bufferSize, "%s%dh%02dm%s", prefix,
            remainingSeconds / 3600, (remainingSeconds % 3600) / 60, suffix);
    } else {
        snprintf(buffer, bufferSize, "%s%dm%s", prefix,
            remainingSeconds / 60, suffix);
    }
}

void CoolingView::reset() {
    _lastRemainingSeconds = -1;
    _lastIconState = IconState::Unset;
    _lastTemperature = -273.15; // Reset to ensure redraw
}

void CoolingView::drawButtons(bool onCancelSelected) {
    const char* buttons[] = {"Démarrer", "Annuler"};
    IBaseView::drawButtons(buttons, 2, onCancelSelected ? 1 : 0);
}

void CoolingView::drawTitle(const time_t endTime) {
    const tm* tm_end = localtime(&endTime);
    char timeBuffer[34] = {'\0'};
    snprintf(timeBuffer, sizeof(timeBuffer), "D\xC3\xA9marrage de la\npousse \xC3\xA0 %d:%02d", tm_end->tm_hour, tm_end->tm_min);
    IBaseView::drawTitle(timeBuffer);
}