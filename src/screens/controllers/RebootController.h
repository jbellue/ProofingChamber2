#pragma once

#include "../Screen.h"
#include "AppContextDecl.h"
#include "../../services/IRebootService.h"

// Forward declarations
struct DisplayManager;
class RebootView;
struct InputManager;

class RebootController : public Screen {
public:
    RebootController(AppContext* ctx);
    bool update(bool forceRedraw = false) override;

private:
    RebootView* _view;
    InputManager* _inputManager;
    services::IRebootService* _rebootService;
    bool _onCancelButton;
    AppContext* _ctx;

    void beginImpl() override;
};
