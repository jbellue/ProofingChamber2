#pragma once

#include "BaseController.h"
#include "IDisplayManager.h"
#include "AppContextDecl.h"
// Storage interface for reading saved timezone
#include "../services/IStorage.h"

class Initialization : public BaseController {
public:
    Initialization(AppContext* ctx);
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    IDisplayManager* _display;
    services::INetworkService* _networkService;
    services::IStorage* _storage;
    void drawScreen();
    void beginImpl() override;
};
