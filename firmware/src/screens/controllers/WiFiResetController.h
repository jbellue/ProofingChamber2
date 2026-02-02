#pragma once

#include "../BaseController.h"
#include "../../AppContextDecl.h"
#include "../../services/INetworkService.h"
#include "../../services/IRebootService.h"
#include "../../IInputManager.h"
#include "../views/WiFiResetView.h"

class WiFiResetController : public BaseController {
public:
    WiFiResetController(AppContext* ctx);
    void begin();
    bool update(bool forceRedraw = false) override;
private:
    services::INetworkService* _networkService;
    services::IRebootService* _rebootService;
    WiFiResetView* _view;
    bool _onCancelButton;
    void beginImpl() override;
};
