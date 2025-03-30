#pragma once

#include <U8g2lib.h>

class DisplayManager {
public:
    DisplayManager(U8G2* display);
    void begin();
    void update();
    void clear();
    void drawStr(uint8_t x, uint8_t y, const char* str);
    void drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    void drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r);
    void setDrawColor(uint8_t color);
    uint8_t getStrWidth(const char* str) const;
    uint8_t getUTF8Width(const char* str) const;
    uint8_t getAscent() const;
    uint8_t getDescent() const;
    uint8_t getDisplayWidth() const;
    uint8_t getDisplayHeight() const;
    uint8_t getWidth() const;
    void sendBuffer();
    void clearBuffer();
    void setFont(const uint8_t* font);
    void drawUTF8(uint8_t x, uint8_t y, const char* str);
    void drawHLine(uint8_t x, uint8_t y, uint8_t w);
    void setFontMode(uint8_t mode);
    void setBitmapMode(uint8_t mode);
    void drawXBMP(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap);

    void setCursor(const uint8_t x, const uint8_t y);
    void print(const char* str);

    uint8_t drawTitle(const char* title, const uint8_t y = 10);
    U8G2* getDisplay() const { return _display; }
private:
    U8G2* _display;
};
