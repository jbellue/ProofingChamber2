#pragma once

#include "../Screen.h"
#include "AppContextDecl.h"
#include "InputManager.h"
#include "SimpleTime.h"
#include "../views/AdjustTimeView.h"

class AdjustTimeController : public Screen {
public:
    AdjustTimeController(AppContext* ctx);
    void begin(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime);
    void beginImpl() override;
    void prepare(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime);
    bool update(bool forceRedraw = false) override;
    struct tm getTime() const;
private:
    enum class SelectedItem {
        Hours,
        Minutes,
        Ok,
        Cancel
    } _selectedItem;
    void beginImpl(const char* title, Screen* coolingScreen, Screen* menuScreen, const SimpleTime& startTime);
    const char* _title;
    SimpleTime _startingTime;
    SimpleTime _currentTime;
    uint8_t _valueY;
    AppContext* _ctx;
    InputManager* _inputManager;
    AdjustTimeView* _view;
    Screen* _coolingScreen;
    Screen* _menuScreen;
    SimpleTime getAdjustedTime(bool isHours, bool increment) const;
    bool isTimeValid(const SimpleTime& t) const;
};
