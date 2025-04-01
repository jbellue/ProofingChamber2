#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "Storage.h"
#include "SimpleTime.h"

class AdjustTime : public Screen {
public:
    AdjustTime(DisplayManager* display, InputManager* inputManager);
    void begin(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime);
    void beginImpl() override {};
    bool update(bool forceRedraw = false) override;
    struct tm getTime() const {
        struct tm timeinfo;
        timeinfo.tm_hour = _currentTime.hours;
        timeinfo.tm_min = _currentTime.minutes;
        timeinfo.tm_mday = _currentTime.days;
        return timeinfo;
    }

private:
    enum class SelectedItem {
        Hours,
        Minutes,
        Ok,
        Cancel
    } _selectedItem;

    void beginImpl(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime);
    void drawTime();
    void drawHighlight();
    void drawButtons();

    Storage _storage;
    const char* _title;
    SimpleTime _startingTime;
    SimpleTime _currentTime;
    uint8_t _valueY;

    DisplayManager* _display;
    InputManager* _inputManager;
    const char* _startTime;

    Screen* _coolingScreen;
    Screen* _menuScreen;

    SimpleTime getAdjustedTime(bool isHours, bool increment) const;
    bool isTimeValid(const SimpleTime& t) const;

};
