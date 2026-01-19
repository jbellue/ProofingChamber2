#pragma once

#include "../BaseController.h"
#include "AppContextDecl.h"
#include "../../services/IRebootService.h"

// Forward declarations
class IDisplayManager;
class RebootView;
class IInputManager;

class RebootController : public BaseController {
public:
    RebootController(AppContext* ctx);
    bool update(bool forceRedraw = false) override;

private:
    RebootView* _view;
    services::IRebootService* _rebootService;
    bool _onCancelButton;

    void beginImpl() override;
};
