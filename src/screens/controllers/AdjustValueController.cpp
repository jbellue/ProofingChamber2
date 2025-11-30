#include "AdjustValueController.h"
#include "../views/AdjustValueView.h"
#include "DebugUtils.h"
#include "icons.h"

AdjustValueController::AdjustValueController(AppContext* ctx) :
    _view(nullptr),
    _inputManager(nullptr),
    _storage(nullptr),
    _ctx(ctx)
{}

void AdjustValueController::begin(const char* title, const char* path) {
    beginImpl(title, path);
}

void AdjustValueController::prepare(const char* title, const char* path) {
    _title = title;
    _path = path;
}

void AdjustValueController::beginImpl() {
    beginImpl(_title, _path);
}

void AdjustValueController::beginImpl(const char* title, const char* path) {
    _title = title;
    _path = path;
    // Obtain storage from the AppContext at begin time (ctx is populated in setup)
    if (_ctx && _ctx->storage) {
        _storage = _ctx->storage;
    }
    // Late-bind view and input
    if (_ctx) {
        _view = _ctx->adjustValueView;
        if (!_inputManager) _inputManager = _ctx->input;
    }
    if (_storage) {
        _currentValue = _storage->readInt(path, 0); // Load initial value
    } else {
        _currentValue = 0;
    }
    if (_inputManager) _inputManager->resetEncoderPosition();

    // Update the view immediately
    _view->clear();
    _valueY = _ctx->display->drawTitle(title);
    const char* buttons[] = {"OK"};
    _view->drawButtons(buttons, 1, 0);
}

bool AdjustValueController::update(bool shouldRedraw) {
    // Handle encoder button press to confirm and save
    if (_inputManager && _inputManager->isButtonPressed()) {
        DEBUG_PRINTLN("AdjustValue: Button pressed, saving value.");
        if (_storage) {
            _storage->writeInt(_path, _currentValue);
        }
        DEBUG_PRINTLN("AdjustValue: Value saved, exiting screen.");
        return false;
    }
    // Handle encoder rotation
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (shouldRedraw || encoderDirection != InputManager::EncoderDirection::None) {
        if (encoderDirection == InputManager::EncoderDirection::Clockwise) {
            _currentValue += 1;
        } else if (encoderDirection == InputManager::EncoderDirection::CounterClockwise) {
            _currentValue -= 1;
        }
        _view->drawValue(_currentValue, _valueY);
        shouldRedraw = true;
    }
    if (shouldRedraw) {
        _view->sendBuffer();
    }
    return true;
}
