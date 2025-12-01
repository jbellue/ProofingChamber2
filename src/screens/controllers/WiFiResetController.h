#pragma once

#include "../Screen.h"
#include "AppContextDecl.h"
#include "../../services/INetworkService.h"
#include "../../services/IRebootService.h"
#include "IInputManager.h"
#include "../views/WiFiResetView.h"

class WiFiResetController : public Screen {
public:
    WiFiResetController(AppContext* ctx);
    void begin();
    bool update(bool forceRedraw = false) override;
private:
    AppContext* _ctx;
    IInputManager* _inputManager;
    services::INetworkService* _networkService;
    services::IRebootService* _rebootService;
    WiFiResetView* _view;
    bool _onCancelButton;
    void beginImpl() override;
};
