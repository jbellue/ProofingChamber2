#pragma once

#include <Arduino.h>

class IDisplayManager {
public:
    virtual ~IDisplayManager() = default;

    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void clear() = 0;
    virtual void drawStr(uint8_t x, uint8_t y, const char* str) = 0;
    virtual void drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h) = 0;
    virtual void drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) = 0;
    virtual void setDrawColor(uint8_t color) = 0;
    virtual uint8_t getStrWidth(const char* str) = 0;
    virtual uint8_t getUTF8Width(const char* str) = 0;
    virtual uint8_t getAscent() = 0;
    virtual uint8_t getDescent() = 0;
    virtual uint8_t getDisplayWidth() = 0;
    virtual uint8_t getDisplayHeight() = 0;
    virtual uint8_t getWidth() = 0;
    virtual void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2) = 0;
    virtual void sendBuffer() = 0;
    virtual void clearBuffer() = 0;
    virtual void setFont(const uint8_t* font) = 0;
    virtual void drawUTF8(uint8_t x, uint8_t y, const char* str) = 0;
    virtual void drawHLine(uint8_t x, uint8_t y, uint8_t w) = 0;
    virtual void drawVLine(uint8_t x, uint8_t y, uint8_t h) = 0;
    virtual void setFontMode(uint8_t mode) = 0;
    virtual void setBitmapMode(uint8_t mode) = 0;
    virtual void drawXBMP(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t* bitmap) = 0;
    virtual void setCursor(const uint8_t x, const uint8_t y) = 0;
    virtual void print(const char* str) = 0;
    virtual uint8_t drawTitle(const char* title, const uint8_t y = 10) = 0;
    virtual void drawButtons(const char* buttonTexts[], uint8_t buttonCount, int8_t selectedButton = -1) = 0;
    virtual void setDisplayUpdateCallback(std::function<void(const String&)> callback) = 0;
};
