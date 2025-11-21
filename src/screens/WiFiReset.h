#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "AppContextDecl.h"

class WiFiReset : public Screen {
public:
    WiFiReset(AppContext* ctx);
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    services::INetworkService* _networkService;
    services::IRebootService* _rebootService;
    AppContext* _ctx;
    bool _onCancelButton;
    void drawScreen();
    void beginImpl() override;
};
