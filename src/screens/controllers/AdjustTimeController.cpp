#include "AdjustTimeController.h"
#include "../views/AdjustTimeView.h"
#include "DebugUtils.h"
#include "../../MenuActions.h"

AdjustTimeController::AdjustTimeController(AppContext* ctx)
    : _ctx(ctx), _inputManager(nullptr), _view(nullptr), _selectedItem(SelectedItem::Hours), _valueY(0), _coolingController(nullptr), _menuScreen(nullptr) {}


void AdjustTimeController::prepare(const char* title, CoolingController* coolingController, Screen* menuScreen, const SimpleTime& startTime, TimeMode mode) {
    _title = title;
    _coolingController = coolingController;
    _menuScreen = menuScreen;
    _startingTime = startTime;
    _currentTime = startTime;
    _timeMode = mode;
}

void AdjustTimeController::beginImpl() {
    DEBUG_PRINTLN("AdjustTimeController::beginImpl called");
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_view) {
            _view = _ctx->adjustTimeView;
        }
    }
    if (_inputManager) _inputManager->resetEncoderPosition();
    _selectedItem = SelectedItem::Hours;

    // Clear display before drawing title and rest of screen
    if (_ctx && _ctx->display) {
        _ctx->display->clear();
        _valueY = _ctx->display->drawTitle(_title);
    } else {
        _valueY = 20;
    }
    _view->drawHighlight(static_cast<uint8_t>(_selectedItem), _valueY);
    _view->drawTime(_currentTime, _valueY);
    _view->drawButtons(-1);
    _view->sendBuffer();
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

bool AdjustTimeController::update(bool shouldRedraw) {
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (encoderDirection != InputManager::EncoderDirection::None) {
        switch (_selectedItem)
        {
            case SelectedItem::Hours:
            case SelectedItem::Minutes: {
                SimpleTime newTime = getAdjustedTime(
                    _selectedItem == SelectedItem::Hours,
                    encoderDirection == InputManager::EncoderDirection::Clockwise
                );
                if (isTimeValid(newTime)) {
                    _currentTime = newTime;
                    _view->drawTime(_currentTime, _valueY);
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
        shouldRedraw = true;
    }
    if (_inputManager && _inputManager->isButtonPressed()) {
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
        _view->drawHighlight(static_cast<uint8_t>(_selectedItem), _valueY);
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
