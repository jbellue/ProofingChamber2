#include "ProofingView.h"
#include "../../DebugUtils.h"
#include "../../icons.h"

bool ProofingView::drawTime(const time_t diffSeconds) {
    if (diffSeconds - _lastTimeDrawn < 60) {
        return false; // No change, skip redraw
    }
    _lastTimeDrawn = diffSeconds;
    const int total_minutes = diffSeconds / 60;
    const int hours = total_minutes / 60;
    const int minutes = total_minutes % 60;

    char timeBuffer[8] = {'\0'};
    if (hours > 0) {
        snprintf(timeBuffer, sizeof(timeBuffer), "%dh%02dm", hours, minutes);
    } else {
        snprintf(timeBuffer, sizeof(timeBuffer), "%dm", minutes);
    }
    setFont(u8g2_font_ncenB18_tf);

    const uint8_t timeWidth = _display->getStrWidth(timeBuffer);
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2;
    const uint8_t timeY = 36;
    const uint8_t fontHeight = _display->getAscent() - _display->getDescent();

    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY - _display->getAscent(), timeWidth, fontHeight);

    _display->setDrawColor(1);
    _display->drawUTF8(timeX, timeY, timeBuffer);
    return true;
}

bool ProofingView::drawTemperature(const float currentTemp) {
    if (abs(currentTemp - _lastTempDrawn) < 0.1) {
        return false; // No significant change, skip redraw
    }
    _lastTempDrawn = currentTemp;
    char tempBuffer[7] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1f°", currentTemp);

    setFont(u8g2_font_t0_11_tf);
    const uint8_t tempWidth = _display->getUTF8Width("99.9°");
    const uint8_t tempHeight = _display->getAscent() - _display->getDescent();
    const uint8_t tempX = _display->getDisplayWidth() - tempWidth;
    const uint8_t tempY = 44;

    _display->setDrawColor(0);
    _display->drawBox(tempX, tempY - _display->getAscent(), tempWidth, tempHeight);
    _display->setDrawColor(1);

    _display->drawUTF8(tempX, tempY, tempBuffer);
    return true;
}

bool ProofingView::drawIcons(IconState iconState) {
    if (iconState == _lastIconState) {
        return false; // No change, skip redraw
    }
    _lastIconState = iconState;
    const uint8_t proofIconSize = 10;
    const uint8_t iconsX = 2;
    const uint8_t iconY = 23;

    if (iconState == IconState::On) {
        _display->drawXBMP(iconsX, iconY, proofIconSize, proofIconSize, iconProof);
    } else {
        _display->setDrawColor(0);
        _display->drawBox(iconsX, iconY, proofIconSize, proofIconSize);
        _display->setDrawColor(1);
    }
    return true;
}


void ProofingView::drawGraph(Graph& graph) {
    _display->setDrawColor(1);
    graph.draw(_display->getDisplay(), _display->getWidth() - 30, 48);
}

void ProofingView::reset() {
    _lastTempDrawn = -257.0;
    _lastIconState = IconState::Unset;
    _lastTimeDrawn = -1000;
}