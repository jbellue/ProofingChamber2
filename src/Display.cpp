#include "Display.h"

Display::Display() : _display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE) {}

void Display::begin() {
    _display.begin();
    _display.clearBuffer();
    _display.setFont(u8g2_font_6x10_tf); // Set a default font
    _display.setFontRefHeightExtendedText();
    _display.setDrawColor(1); // Set draw color to white
    _display.setFontPosTop();
}

void Display::clear() {
    _display.clearBuffer();
}

void Display::drawRect(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height) {
    _display.drawFrame(x, y, width, height);
}

void Display::setCursor(const uint8_t x, const uint8_t y) {
    _display.setCursor(x, y);
}

void Display::update() {
    _display.sendBuffer();
}
