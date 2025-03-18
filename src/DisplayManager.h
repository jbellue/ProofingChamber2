#pragma once

#include <U8g2lib.h>

class DisplayManager {
public:
    DisplayManager(U8G2_SH1106_128X64_NONAME_F_HW_I2C* display);
    void initialize();
    void clearBuffer();
    void setFont(const uint8_t* font);
    void drawStr(int x, int y, const char* str);
    void sendBuffer();
    void drawUTF8(int x, int y, const char* str);
    void drawXBMP(int x, int y, int width, int height, const uint8_t* bitmap);
    void drawHLine(const uint8_t x, const uint8_t y, const uint8_t width);
    void setDrawColor(int color);
    void drawRBox(int x, int y, int width, int height, int radius);
    void setFontMode(int mode);
    void setBitmapMode(int mode);
    void clear();
    uint8_t getAscent();
    uint8_t getDescent();
    uint8_t getDisplayWidth();
    uint8_t getStrWidth(const char* str);

private:
    U8G2_SH1106_128X64_NONAME_F_HW_I2C* _display;
};
