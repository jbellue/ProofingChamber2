// Mock U8g2lib.h for native testing
#pragma once

#include <stdint.h>

// Mock U8G2 class
class U8G2 {
public:
    void setDrawColor(int) {}
    void drawBox(uint8_t, uint8_t, uint8_t, uint8_t) {}
    void drawFrame(uint8_t, uint8_t, uint8_t, uint8_t) {}
    void drawPixel(uint8_t, uint8_t) {}
};
