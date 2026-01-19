#pragma once

#include "../BaseController.h"
#include "IDisplayManager.h"
#include "IInputManager.h"
#include "services/IStorage.h"

#include "../views/AdjustValueView.h"
#include "AppContextDecl.h"

class AdjustValueController : public BaseController {
public:
    AdjustValueController(AppContext* ctx);
    void beginImpl() override;
    void prepare(const char* title, const char* path);
    bool update(bool forceRedraw = false) override;
private:
    const char* _title;
    const char* _path;
    uint8_t _valueY;
    int _currentValue;
    AdjustValueView* _view;
    services::IStorage* _storage;
};
