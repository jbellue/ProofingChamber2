#pragma once

#include "../Screen.h"
#include "AppContextDecl.h"
#include "../../services/IRebootService.h"

// Forward declarations
class IDisplayManager;
class RebootView;
class IInputManager;

class RebootController : public Screen {
public:
    RebootController(AppContext* ctx);
    bool update(bool forceRedraw = false) override;

private:
    RebootView* _view;
    IInputManager* _inputManager;
    services::IRebootService* _rebootService;
    bool _onCancelButton;
    AppContext* _ctx;

    void beginImpl() override;
};
