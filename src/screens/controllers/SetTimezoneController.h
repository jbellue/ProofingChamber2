#pragma once

#include "../Screen.h"
#include "AppContextDecl.h"
#include "IInputManager.h"
#include "../views/SetTimezoneView.h"

class SetTimezoneController : public Screen {
public:
    SetTimezoneController(AppContext* ctx);
    bool update(bool forceRedraw = false) override;
private:
    AppContext* _ctx;
    IInputManager* _inputManager;
    SetTimezoneView* _view;
    void beginImpl() override;
};
