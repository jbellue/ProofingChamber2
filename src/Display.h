#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>

#define SCREEN_WIDTH        124
#define SCREEN_HEIGHT       68
#define SSD1306_I2C_ADDRESS 0x3C

class Display {
public:
    Display();
    void begin();
    void clear();
    void drawRect(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height);
    void setCursor(const uint8_t x, const uint8_t y);

    template <typename T>
    void print(const T& value) {
        _display.print(value);
    }

    // Template for println method
    template <typename T>
    void println(const T& value) {
        _display.println(value);
    }

    void update();

private:
    U8G2_SSD1309_128X64_NONAME0_F_HW_I2C _display; // Use U8g2 for SSD1309
};

#endif