#pragma once

#include "IDisplayManager.h"
#include "DebugUtils.h"

// Null Object Pattern: Safe do-nothing implementation
class NullDisplayManager : public IDisplayManager {
public:
    static NullDisplayManager& getInstance() {
        static NullDisplayManager instance;
        return instance;
    }

    void begin() override {
        DEBUG_PRINTLN("NullDisplayManager: begin() called - no-op");
    }

    void update() override {
        // Silent no-op - called frequently
    }

    void clear() override {
        // Silent no-op
    }

    void drawStr(uint8_t x, uint8_t y, const char* str) override {
        // Silent no-op - called frequently
    }

    void drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h) override {
        // Silent no-op
    }

    void drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) override {
        // Silent no-op
    }

    void setDrawColor(uint8_t color) override {
        // Silent no-op
    }

    uint8_t getStrWidth(const char* str) override {
        return 0;
    }

    uint8_t getUTF8Width(const char* str) override {
        return 0;
    }

    uint8_t getAscent() override {
        return 0;
    }

    uint8_t getDescent() override {
        return 0;
    }

    uint8_t getDisplayWidth() override {
        return 128; // Safe default
    }

    uint8_t getDisplayHeight() override {
        return 64; // Safe default
    }

    uint8_t getWidth() override {
        return 128; // Safe default
    }

    void sendBuffer() override {
        // Silent no-op
    }

    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2) override {
        // Silent no-op
    }
    
    void clearBuffer() override {
        // Silent no-op
    }

    void setFont(const uint8_t* font) override {
        // Silent no-op
    }

    void drawUTF8(uint8_t x, uint8_t y, const char* str) override {
        // Silent no-op
    }

    void drawHLine(uint8_t x, uint8_t y, uint8_t w) override {
        // Silent no-op
    }

    void drawVLine(uint8_t x, uint8_t y, uint8_t h) override {
        // Silent no-op
    }

    void setFontMode(uint8_t mode) override {
        // Silent no-op
    }

    void setBitmapMode(uint8_t mode) override {
        // Silent no-op
    }

    void drawXBMP(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap) override {
        // Silent no-op
    }

    void setCursor(const uint8_t x, const uint8_t y) override {
        // Silent no-op
    }

    void print(const char* str) override {
        // Silent no-op
    }

    uint8_t drawTitle(const char* title, const uint8_t y = 10) override {
        DEBUG_PRINT("NullDisplayManager: drawTitle('");
        DEBUG_PRINT(title);
        DEBUG_PRINTLN("') called - no-op");
        return y + 10;
    }

    void drawButtons(const char* buttonTexts[], uint8_t buttonCount, int8_t selectedButton = -1) override {
        DEBUG_PRINTLN("NullDisplayManager: drawButtons() called - no-op");
    }

private:
    NullDisplayManager() = default;
    NullDisplayManager(const NullDisplayManager&) = delete;
    NullDisplayManager& operator=(const NullDisplayManager&) = delete;
};
