#include "SetTimezone.h"
#include "icons.h"

SetTimezone::SetTimezone(AppContext* ctx) :
    _display(nullptr), _inputManager(nullptr), _ctx(ctx)
{}

void SetTimezone::begin() {
    beginImpl();
}

void SetTimezone::beginImpl() {
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_display) _display = _ctx->display;
    }
    if (_inputManager) _inputManager->resetEncoderPosition();
    if (_display) {
        _display->clear();
        _display->drawTitle("Buy PRO to unlock", 20);
    }
}

bool SetTimezone::update(bool forceRedraw) {
    // Handle encoder rotation
    bool redraw = forceRedraw;
    if (redraw) {
        drawScreen(); // Only redraw if necessary
    }
    return !(_inputManager && _inputManager->isButtonPressed());
}

void SetTimezone::drawScreen() {
    if (_display) {
        _display->drawButton("OK", true);
        _display->sendBuffer();
    }
}
