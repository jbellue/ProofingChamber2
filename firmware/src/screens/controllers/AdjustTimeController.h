#pragma once

#include "../BaseController.h"
#include "../../AppContextDecl.h"
#include "../../IInputManager.h"
#include "../../SimpleTime.h"
#include "../views/AdjustTimeView.h"

#include "./CoolingController.h"

enum class TimeMode {
    ProofIn,
    ProofAt
};

class AdjustTimeController : public BaseController {
public:
    AdjustTimeController(AppContext* ctx);
    void beginImpl() override;
    void prepare(const char* title, CoolingController* coolingController, BaseController* menuScreen, const SimpleTime& startTime, TimeMode mode);
    bool update(bool forceRedraw = false) override;
    struct tm getTime() const;
private:
    enum class SelectedItem {
        Hours,
        Minutes,
        Ok,
        Cancel
    } _selectedItem;
    const char* _title;
    TimeMode _timeMode;
    SimpleTime _startingTime;
    SimpleTime _currentTime;
    uint8_t _titleHeight;
    AdjustTimeView* _view;
    CoolingController* _coolingController;
    BaseController* _menuScreen;
    SimpleTime getAdjustedTime(bool isHours, bool increment) const;
    bool isTimeValid(const SimpleTime& t) const;
    bool handleEncoderInput(IInputManager::EncoderDirection direction);
};
