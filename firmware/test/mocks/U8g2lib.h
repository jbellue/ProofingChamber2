// Mock U8g2lib.h for unit testing
#pragma once

#include <stdint.h>

class U8G2 {
public:
    void begin() {}
    void clearBuffer() {}
    void sendBuffer() {}
    void setFont(const uint8_t *font) {}
    void setCursor(uint16_t x, uint16_t y) {}
    void print(const char *s) {}
    void drawStr(uint16_t x, uint16_t y, const char *s) {}
    void drawBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {}
    void drawFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {}
    uint16_t getStrWidth(const char *s) { return 0; }
    uint16_t getMaxCharHeight() { return 8; }
};

class U8G2_SH1106_128X64_NONAME_F_HW_I2C : public U8G2 {
public:
    U8G2_SH1106_128X64_NONAME_F_HW_I2C(uint8_t rotation, uint8_t reset = 255, uint8_t clock = 255, uint8_t data = 255) {}
};

// Font declarations - just empty arrays
extern const uint8_t u8g2_font_6x10_tf[];
extern const uint8_t u8g2_font_7x14_tf[];
