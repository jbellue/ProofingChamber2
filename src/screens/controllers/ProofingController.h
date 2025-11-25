#pragma once

#include "../Screen.h"
#include "AppContextDecl.h"
#include "../../Graph.h"

// Forward
class ProofingView;
struct InputManager;
struct TemperatureController;

class ProofingController : public Screen {
public:
    explicit ProofingController(AppContext* ctx);
    ~ProofingController();
    void beginImpl() override;
    bool update(bool forceRedraw = false) override;

private:
    ProofingView* _view;
    InputManager* _inputManager;
    AppContext* _ctx;
    time_t _startTime;
    time_t _lastTemperatureUpdate;
    time_t _lastGraphUpdate;
    time_t _previousDiffSeconds;
    float _previousTemp;
    float _currentTemp;
    bool _isIconOn;
    Graph _temperatureGraph;
    TemperatureController* _temperatureController;

    void drawTime();
    void drawTemperature();
};
