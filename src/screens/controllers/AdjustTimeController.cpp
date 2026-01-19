#include "AdjustTimeController.h"
#include "../views/AdjustTimeView.h"
#include "DebugUtils.h"
#include "../../MenuActions.h"

AdjustTimeController::AdjustTimeController(AppContext* ctx)
    : BaseController(ctx), _view(nullptr), _selectedItem(SelectedItem::Hours), _titleHeight(0), _coolingController(nullptr), _menuScreen(nullptr) {}


void AdjustTimeController::prepare(const char* title, CoolingController* coolingController, BaseController* menuScreen, const SimpleTime& startTime, TimeMode mode) {
    _title = title;
    _coolingController = coolingController;
    _menuScreen = menuScreen;
    _startingTime = startTime;
    _currentTime = startTime;
    _timeMode = mode;
}

void AdjustTimeController::beginImpl() {
    DEBUG_PRINTLN("AdjustTimeController::beginImpl called");
    initializeInputManager();
    
    AppContext* ctx = getContext();
    if (ctx) {
        _view = ctx->adjustTimeView;
    }
    _selectedItem = SelectedItem::Hours;

    // Clear display before drawing title and rest of screen
    _titleHeight = _view->start(_title, _currentTime, static_cast<uint8_t>(_selectedItem), -1);
}

bool AdjustTimeController::isTimeValid(const SimpleTime& t) const {
    if (t.days > 0) return true;
    if (_startingTime.hours == 0 && _startingTime.minutes == 0) {
        return t.days >= 0 && t.hours >= 0 && t.minutes >= 0;
    }
    return t >= _startingTime;
}

SimpleTime AdjustTimeController::getAdjustedTime(bool isHours, bool increment) const {
    SimpleTime newTime = _currentTime;
    if (increment) {
        isHours ? newTime.incrementHours() : newTime.incrementMinutes();
    } else {
        isHours ? newTime.decrementHours() : newTime.decrementMinutes();
    }
    return newTime;
}

bool AdjustTimeController::handleEncoderInput(IInputManager::EncoderDirection direction) {
    if (direction == IInputManager::EncoderDirection::None) {
        return false;
    }
    switch (_selectedItem)
    {
    case SelectedItem::Hours:
    case SelectedItem::Minutes: {
        SimpleTime newTime = getAdjustedTime(
            _selectedItem == SelectedItem::Hours,
            direction == IInputManager::EncoderDirection::Clockwise
        );
        if (isTimeValid(newTime)) {
            _currentTime = newTime;
            _view->drawTime(_currentTime, _titleHeight);
        }
        break;
    }
    case SelectedItem::Ok:
        _selectedItem = SelectedItem::Cancel;
        _view->drawButtons(1);
        break;
    case SelectedItem::Cancel:
        _selectedItem = SelectedItem::Ok;
        _view->drawButtons(0);
        break;
    }
    return true;
}
bool AdjustTimeController::update(bool shouldRedraw) {
    IInputManager* inputManager = getInputManager();
    shouldRedraw |= handleEncoderInput(inputManager->getEncoderDirection());
    if (inputManager->isButtonPressed()) {
        switch (_selectedItem)
        {
            case SelectedItem::Hours:
                _selectedItem = SelectedItem::Minutes;
                break;
        case SelectedItem::Minutes:
            _selectedItem = SelectedItem::Ok;
            _view->drawButtons(0);
            break;
        case SelectedItem::Ok:
            // Update the appropriate static variable based on the mode
            if (_timeMode == TimeMode::ProofIn) {
                MenuActions::s_proofInTime = _currentTime;
            } else if (_timeMode == TimeMode::ProofAt) {
                MenuActions::s_proofAtTime = _currentTime;
            }
            setNextScreen(_coolingController);
            return false;
        case SelectedItem::Cancel:
            setNextScreen(_menuScreen);
            return false;
        }
        _view->drawHighlight(static_cast<uint8_t>(_selectedItem), _titleHeight);
        shouldRedraw = true;
    }
    if (shouldRedraw) {
        _view->sendBuffer();
    }
    return true;
}

struct tm AdjustTimeController::getTime() const {
    struct tm timeinfo;
    timeinfo.tm_hour = _currentTime.hours;
    timeinfo.tm_min = _currentTime.minutes;
    timeinfo.tm_mday = _currentTime.days;
    return timeinfo;
}
