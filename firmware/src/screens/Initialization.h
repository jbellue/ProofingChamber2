#pragma once

#include "BaseController.h"
#include "../IDisplayManager.h"
#include "../AppContextDecl.h"
#include "../services/IStorage.h"
#include "../utils/LineRingBuffer.h"

class Initialization : public BaseController {
public:
    Initialization(AppContext* ctx);
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    IDisplayManager* _display;
    services::INetworkService* _networkService;
    services::IStorage* _storage;
    services::IWebServerService* _webServerService;
    void drawScreen();
    void beginImpl() override;

    LineRingBuffer<5> _buffer;
    const uint8_t _baseY = 10;       // first line Y
    const uint8_t _lineSpacing = 12; // distance between lines

    void renderBuffer();
};
