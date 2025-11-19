#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "services/IStorage.h"
#include "AppContextDecl.h"

class AdjustValue : public Screen {
public:
    AdjustValue(AppContext* ctx);
    void begin(const char* title, const char* path);
    void beginImpl() override;
    void prepare(const char* title, const char* path);
    bool update(bool forceRedraw = false) override;
private:
    void drawValue();
    void drawButton();
    void beginImpl(const char* title, const char* path);

    const char* _title;
    const char* _path;
    uint8_t _valueY;
    int _currentValue;
    DisplayManager* _display;
    InputManager* _inputManager;
        services::IStorage* _storage;
        AppContext* _ctx;
    const char* _startTime;
};
