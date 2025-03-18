#include "DisplayManager.h"

DisplayManager::DisplayManager(U8G2_SH1106_128X64_NONAME_F_HW_I2C* display) : _display(display) {}

void DisplayManager::initialize() {
    _display->begin();
    clearBuffer();
    setFont(u8g2_font_t0_11_tf);
}

void DisplayManager::clearBuffer() {
    _display->clearBuffer();
}

void DisplayManager::setFont(const uint8_t* font) {
    _display->setFont(font);
}

void DisplayManager::drawStr(int x, int y, const char* str) {
    _display->drawStr(x, y, str);
}

void DisplayManager::sendBuffer() {
    _display->sendBuffer();
}

void DisplayManager::drawUTF8(int x, int y, const char* str) {
    _display->drawUTF8(x, y, str);
}

void DisplayManager::drawXBMP(int x, int y, int width, int height, const uint8_t* bitmap) {
    _display->drawXBMP(x, y, width, height, bitmap);
}

void DisplayManager::drawHLine(const uint8_t x, const uint8_t y, const uint8_t width) {
    _display->drawHLine(x, y, width);
}

void DisplayManager::setDrawColor(int color) {
    _display->setDrawColor(color);
}

void DisplayManager::drawRBox(int x, int y, int width, int height, int radius) {
    _display->drawRBox(x, y, width, height, radius);
}

void DisplayManager::setFontMode(int mode) {
    _display->setFontMode(mode);
}

void DisplayManager::setBitmapMode(int mode) {
    _display->setBitmapMode(mode);
}

void DisplayManager::clear() {
    _display->clear();
}

uint8_t DisplayManager::getAscent() {
    return _display->getAscent();
}

uint8_t DisplayManager::getDescent() {
    return _display->getDescent();
}

uint8_t DisplayManager::getDisplayWidth() {
    return _display->getDisplayWidth();
}

uint8_t DisplayManager::getStrWidth(const char* str) {
    return _display->getStrWidth(str);
}