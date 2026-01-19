#include "DisplayManager.h"

DisplayManager::DisplayManager(const u8g2_cb_t * rotation) : _display(rotation) {}

void DisplayManager::begin() {
    _display.begin();
}

void DisplayManager::update() {
    _display.firstPage();
    do {
        // Draw the current page
    } while (_display.nextPage());
}

void DisplayManager::clear() {
    _display.clear();
}

void DisplayManager::clearBuffer() {
    _display.clearBuffer();
}

void DisplayManager::drawStr(uint8_t x, uint8_t y, const char* str) {
    _display.drawUTF8(x, y, str);
}

void DisplayManager::drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    _display.drawBox(x, y, w, h);
}

void DisplayManager::drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) {
    _display.drawRBox(x, y, w, h, r);
}

void DisplayManager::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    _display.drawTriangle(x0, y0, x1, y1, x2, y2);
}

void DisplayManager::setDrawColor(uint8_t color) {
    _display.setDrawColor(color);
}

uint8_t DisplayManager::getStrWidth(const char* str) {
    return _display.getStrWidth(str);
}

uint8_t DisplayManager::getUTF8Width(const char* str) {
    return _display.getUTF8Width(str);
}

uint8_t DisplayManager::getWidth() {
    return _display.getWidth();
}

uint8_t DisplayManager::getAscent() {
    return _display.getAscent();
}

uint8_t DisplayManager::getDescent() {
    return _display.getDescent();
}

uint8_t DisplayManager::getDisplayWidth() {
    return _display.getDisplayWidth();
}

uint8_t DisplayManager::getDisplayHeight() {
    return _display.getDisplayHeight();
}

void DisplayManager::sendBuffer() {
    _display.sendBuffer();
}

void DisplayManager::setFont(const uint8_t* font) {
    _display.setFont(font);
}

void DisplayManager::drawUTF8(uint8_t x, uint8_t y, const char* str) {
    _display.drawUTF8(x, y, str);
}

void DisplayManager::drawHLine(uint8_t x, uint8_t y, uint8_t w) {
    _display.drawHLine(x, y, w);
}

void DisplayManager::drawVLine(uint8_t x, uint8_t y, uint8_t h) {
    _display.drawVLine(x, y, h);
}

void DisplayManager::setFontMode(uint8_t mode) {
    _display.setFontMode(mode);
}

void DisplayManager::setBitmapMode(uint8_t mode) {
    _display.setBitmapMode(mode);
}

void DisplayManager::drawXBMP(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap) {
    _display.drawXBMP(x, y, w, h, bitmap);
}

void DisplayManager::setCursor(const uint8_t x, const uint8_t y) {
    _display.setCursor(x, y);
}

void DisplayManager::print(const char* str) {
    _display.print(str);
}

uint8_t DisplayManager::drawTitle(const char* title, const uint8_t y) {
    _display.setFont(u8g2_font_t0_11_tf);
    _display.setDrawColor(1);
    const uint8_t lineHeight = _display.getAscent() - _display.getDescent() + 2; // Line height (font height + spacing)
    const uint8_t displayWidth = _display.getDisplayWidth(); // Get the display width

    // Split the title into lines based on EOL or CR
    char titleCopy[100]; // Copy the title to a mutable buffer
    strncpy(titleCopy, title, sizeof(titleCopy));
    titleCopy[sizeof(titleCopy) - 1] = '\0'; // Ensure null termination

    char* line = strtok(titleCopy, "\n"); // Split the title into lines
    uint8_t currentY = y; // Initial Y position for drawing

    while (line != nullptr) {
        // Draw the current line centered on the display
        _display.drawUTF8((displayWidth - _display.getUTF8Width(line)) / 2, currentY, line);
        currentY += lineHeight; // Move Y position down

        // Get the next line
        line = strtok(nullptr, "\n");
    }
    return currentY + lineHeight;
}



void DisplayManager::drawButtons(const char* buttonTexts[], uint8_t buttonCount, int8_t selectedButton) {
    setFont(u8g2_font_t0_11_tf);
    const uint8_t padding = 3;
    const uint8_t screenHeight = getDisplayHeight();
    const uint8_t screenWidth = getDisplayWidth();
    const uint8_t buttonsY = screenHeight - padding;
    const uint8_t buttonHeight = 13;
    setDrawColor(0);
    drawBox(0, screenHeight - buttonHeight, screenWidth, buttonHeight);
    if (buttonCount == 1) {
        const uint8_t sidePadding = 7;
        uint8_t textWidth = getUTF8Width(buttonTexts[0]);
        uint8_t buttonWidth = textWidth + 2 * sidePadding;
        uint8_t buttonX = (screenWidth - buttonWidth) / 2;
        setDrawColor(1);
        drawUTF8(buttonX + sidePadding, buttonsY, buttonTexts[0]);
        if (selectedButton == 0) {
            setDrawColor(2);
            drawRBox(buttonX, screenHeight - buttonHeight, buttonWidth, buttonHeight, 1);
        }
    } else {
        uint8_t buttonAreaWidth = screenWidth / buttonCount;
        for (uint8_t i = 0; i < buttonCount; ++i) {
            setDrawColor(1);
            uint8_t textWidth = getUTF8Width(buttonTexts[i]);
            drawUTF8(i * buttonAreaWidth + (buttonAreaWidth - textWidth) / 2, buttonsY, buttonTexts[i]);
            if (selectedButton == i) {
                setDrawColor(2);
                drawRBox(i * buttonAreaWidth, screenHeight - buttonHeight, buttonAreaWidth, buttonHeight, 1);
            }
        }
    }
}
