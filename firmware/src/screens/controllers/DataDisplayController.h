#pragma once

#include "../BaseController.h"
#include "../../AppContextDecl.h"

class ITemperatureController;

class DataDisplayController : public BaseController {
public:
    explicit DataDisplayController(AppContext* ctx) : BaseController(ctx), _view(nullptr) {}
    void beginImpl() override;
    bool update(bool forceRedraw = false) override;
private:
    DataDisplayView* _view;
    uint32_t _lastUpdateTime = 0;
};