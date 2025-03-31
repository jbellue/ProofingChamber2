#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "Storage.h"

class AdjustTime : public Screen {
public:
    AdjustTime(DisplayManager* display, InputManager* inputManager);
    void begin(const char* title, Screen* coolingScreen, Screen* menuScreen, const uint8_t startH = 0, const uint8_t startM = 0);
    void beginImpl() override {};
    bool update(bool forceRedraw = false) override;
    struct tm getTime() const {
        struct tm timeinfo;
        timeinfo.tm_hour = _currentHours;
        timeinfo.tm_min = _currentMinutes;
        timeinfo.tm_mday = _currentDays;
        return timeinfo;
    }

private:
    enum class SelectedItem {
        Hours,
        Minutes,
        Ok,
        Cancel
    } _selectedItem;
    void beginImpl(const char* title, Screen* coolingScreen, Screen* menuScreen, const uint8_t startH = 0, const uint8_t startM = 0);
    void drawTime();
    void drawHighlight();
    void drawButtons();

    Storage _storage;
    const char* _title;
    uint8_t _currentDays;
    int8_t _currentHours;
    int8_t _currentMinutes;
    uint8_t _valueY;

    DisplayManager* _display;
    InputManager* _inputManager;
    const char* _startTime;

    Screen* _coolingScreen;
    Screen* _menuScreen;
};
