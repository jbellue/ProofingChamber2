#pragma once

#include <U8g2lib.h>
#include "IDisplayManager.h"

class DisplayManager : public IDisplayManager {
public:
    DisplayManager(const u8g2_cb_t * rotation);
    void begin() override;
    void update() override;
    void clear() override;
    void drawStr(uint8_t x, uint8_t y, const char* str) override;
    void drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h) override;
    void drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) override;
    void setDrawColor(uint8_t color) override;
    uint8_t getStrWidth(const char* str) override;
    uint8_t getUTF8Width(const char* str) override;
    uint8_t getAscent() override;
    uint8_t getDescent() override;
    uint8_t getDisplayWidth() override;
    uint8_t getDisplayHeight() override;
    uint8_t getWidth() override;
    void sendBuffer() override;
    void clearBuffer() override;
    void setFont(const uint8_t* font) override;
    void drawUTF8(uint8_t x, uint8_t y, const char* str) override;
    void drawHLine(uint8_t x, uint8_t y, uint8_t w) override;
    void setFontMode(uint8_t mode) override;
    void setBitmapMode(uint8_t mode) override;
    void drawXBMP(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap) override;

    void setCursor(const uint8_t x, const uint8_t y) override;
    void print(const char* str) override;

    uint8_t drawTitle(const char* title, const uint8_t y = 10) override;
    void drawButtons(const char* buttonTexts[], uint8_t buttonCount, int8_t selectedButton = -1) override;
    U8G2* getDisplay() { return &_display; }
private:
    U8G2_SH1106_128X64_NONAME_F_HW_I2C _display;
};
