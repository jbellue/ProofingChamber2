#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "AppContextDecl.h"

class SetTimezone : public Screen {
public:
    SetTimezone(AppContext* ctx);
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    AppContext* _ctx;
    void drawScreen();
    void beginImpl() override;
};
