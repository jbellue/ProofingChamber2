#pragma once

#include "../BaseController.h"
#include "AppContextDecl.h"
#include "IInputManager.h"
#include "../views/SetTimezoneView.h"

class SetTimezoneController : public BaseController {
public:
    SetTimezoneController(AppContext* ctx);
    bool update(bool forceRedraw = false) override;
private:
    SetTimezoneView* _view;
    void beginImpl() override;
};
