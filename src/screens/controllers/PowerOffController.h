#pragma once

#include "../BaseController.h"
#include "AppContextDecl.h"

// Forward declarations
class IDisplayManager;
class PowerOffView;
class IInputManager;

class PowerOffController : public BaseController {
public:
    PowerOffController(AppContext* ctx);
    bool update(bool forceRedraw = false) override;

private:
    PowerOffView* _view;
    bool _onCancelButton;

    void beginImpl() override;
    void performPowerOff();
};
