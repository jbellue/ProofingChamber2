#include "SetTimezone.h"
#include "icons.h"

SetTimezone::SetTimezone(AppContext* ctx) :
    _display(ctx->display), _inputManager(ctx->input), _ctx(ctx)
{}

void SetTimezone::begin() {
    beginImpl();
}

void SetTimezone::beginImpl() {
    _inputManager->resetEncoderPosition();
    _display->clear();
    _display->drawTitle("Buy PRO to unlock", 20);
}

bool SetTimezone::update(bool forceRedraw) {
    // Handle encoder rotation
    bool redraw = forceRedraw;
    if (redraw) {
        drawScreen(); // Only redraw if necessary
    }
    return (!_inputManager->isButtonPressed());
}

void SetTimezone::drawScreen() {
    _display->drawButton("OK", true);
    _display->sendBuffer();
}
