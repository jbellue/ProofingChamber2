#pragma once

#include "Screen.h"
#include "IDisplayManager.h"
#include "AppContextDecl.h"

class Initialization : public Screen {
public:
    Initialization(AppContext* ctx);
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    IDisplayManager* _display;
    services::INetworkService* _networkService;
    AppContext* _ctx;
    void drawScreen();
    void beginImpl() override;
};
