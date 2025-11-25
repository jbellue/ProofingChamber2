#pragma once

#include "../Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "services/IStorage.h"

#include "../views/AdjustValueView.h"
#include "AppContextDecl.h"

class AdjustValueController : public Screen {
public:
    AdjustValueController(AppContext* ctx);
    void begin(const char* title, const char* path);
    void beginImpl() override;
    void prepare(const char* title, const char* path);
    bool update(bool forceRedraw = false) override;
private:
    void beginImpl(const char* title, const char* path);
    const char* _title;
    const char* _path;
    uint8_t _valueY;
    int _currentValue;
    AdjustValueView* _view;
    InputManager* _inputManager;
    services::IStorage* _storage;
    AppContext* _ctx;
};
