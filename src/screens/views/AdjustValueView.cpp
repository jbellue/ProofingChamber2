#include "AdjustValueView.h"

bool AdjustValueView::drawValue(int value, uint8_t valueY) {
    if (value == _lastValueDrawn) {
        return false; // No change, skip redraw
    }
    _lastValueDrawn = value;
    _display->setFont(u8g2_font_ncenB18_tf);
    char buffer[6] = {'\0'};
    const uint8_t writtenChars = sprintf(buffer, "%d", value);
    const uint8_t valueWidth = _display->getUTF8Width(buffer);
    buffer[writtenChars] = 0xC2;
    buffer[writtenChars+1] = 0xB0;
    const uint8_t valueX = (_display->getDisplayWidth() - valueWidth) / 2;
    _display->setDrawColor(0);
    const uint8_t ascent = _display->getAscent();
    _display->drawBox(0, valueY - ascent, _display->getDisplayWidth(), ascent);
    _display->setDrawColor(1);
    _display->drawStr(valueX, valueY, buffer);
    return true;
}

void AdjustValueView::drawButtons() {
    const char* buttons[] = {"OK"};
    IBaseView::drawButtons(buttons, 1, 0);
}

uint8_t AdjustValueView::start(const char* title, const int value) {
    reset();
    clear();
    const uint8_t valueY = _display->drawTitle(title);
    drawValue(value, valueY);
    drawButtons();
    sendBuffer();
    return valueY;
}

void AdjustValueView::reset() {
    _lastValueDrawn = INT32_MIN;
}
