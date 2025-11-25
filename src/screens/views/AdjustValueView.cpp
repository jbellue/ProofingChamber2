#include "AdjustValueView.h"

AdjustValueView::AdjustValueView(DisplayManager* display)
    : _display(display) {}

void AdjustValueView::drawValue(int value, uint8_t valueY) {
    if (!_display) return;
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
}

void AdjustValueView::drawButton(const char* label, bool selected) {
    if (_display) _display->drawButton(label, selected);
}

void AdjustValueView::clear() {
    if (_display) _display->clear();
}

void AdjustValueView::setFont(const uint8_t* font) {
    if (_display) _display->setFont(font);
}

void AdjustValueView::sendBuffer() {
    if (_display) _display->sendBuffer();
}

int AdjustValueView::getDisplayWidth() const {
    return _display ? _display->getDisplayWidth() : 0;
}

uint8_t AdjustValueView::getUTF8Width(const char* str) const {
    return _display ? _display->getUTF8Width(str) : 0;
}

uint8_t AdjustValueView::getAscent() const {
    return _display ? _display->getAscent() : 0;
}
