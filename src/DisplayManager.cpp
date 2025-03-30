#include "DisplayManager.h"

DisplayManager::DisplayManager(U8G2* display) : _display(display) {}

void DisplayManager::begin() {
    _display->begin();
}

void DisplayManager::update() {
    _display->firstPage();
    do {
        // Draw the current page
    } while (_display->nextPage());
}

void DisplayManager::clear() {
    _display->clear();
}

void DisplayManager::clearBuffer() {
    _display->clearBuffer();
}

void DisplayManager::drawStr(uint8_t x, uint8_t y, const char* str) {
    _display->drawUTF8(x, y, str);
}

void DisplayManager::drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    _display->drawBox(x, y, w, h);
}

void DisplayManager::drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) {
    _display->drawRBox(x, y, w, h, r);
}

void DisplayManager::setDrawColor(uint8_t color) {
    _display->setDrawColor(color);
}

uint8_t DisplayManager::getStrWidth(const char* str) const {
    return _display->getStrWidth(str);
}

uint8_t DisplayManager::getUTF8Width(const char* str) const {
    return _display->getUTF8Width(str);
}

uint8_t DisplayManager::getWidth() const {
    return _display->getWidth();
}

uint8_t DisplayManager::getAscent() const {
    return _display->getAscent();
}

uint8_t DisplayManager::getDescent() const {
    return _display->getDescent();
}

uint8_t DisplayManager::getDisplayWidth() const {
    return _display->getDisplayWidth();
}

uint8_t DisplayManager::getDisplayHeight() const {
    return _display->getDisplayHeight();
}

void DisplayManager::sendBuffer() {
    _display->sendBuffer();
}

void DisplayManager::setFont(const uint8_t* font) {
    _display->setFont(font);
}

void DisplayManager::drawUTF8(uint8_t x, uint8_t y, const char* str) {
    _display->drawUTF8(x, y, str);
}

void DisplayManager::drawHLine(uint8_t x, uint8_t y, uint8_t w) {
    _display->drawHLine(x, y, w);
}

void DisplayManager::setFontMode(uint8_t mode) {
    _display->setFontMode(mode);
}

void DisplayManager::setBitmapMode(uint8_t mode) {
    _display->setBitmapMode(mode);
}

void DisplayManager::drawXBMP(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap) {
    _display->drawXBMP(x, y, w, h, bitmap);
}

void DisplayManager::setCursor(const uint8_t x, const uint8_t y) {
    _display->setCursor(x, y);
}

void DisplayManager::print(const char* str) {
    _display->print(str);
}

uint8_t DisplayManager::drawTitle(const char* title, const uint8_t y) {
    _display->setFont(u8g2_font_t0_11_tf);
    _display->setDrawColor(1);
    const uint8_t lineHeight = _display->getAscent() - _display->getDescent() + 2; // Line height (font height + spacing)
    const uint8_t displayWidth = _display->getDisplayWidth(); // Get the display width

    // Split the title into lines based on EOL or CR
    char titleCopy[100]; // Copy the title to a mutable buffer
    strncpy(titleCopy, title, sizeof(titleCopy));
    titleCopy[sizeof(titleCopy) - 1] = '\0'; // Ensure null termination

    char* line = strtok(titleCopy, "\n"); // Split the title into lines
    uint8_t currentY = y; // Initial Y position for drawing

    while (line != nullptr) {
        // Draw the current line centered on the display
        _display->drawUTF8((displayWidth - _display->getStrWidth(line)) / 2, currentY, line);
        currentY += lineHeight; // Move Y position down

        // Get the next line
        line = strtok(nullptr, "\n");
    }
    return currentY + lineHeight;
}