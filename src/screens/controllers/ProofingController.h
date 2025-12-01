#pragma once

#include "../Screen.h"
#include "AppContextDecl.h"
#include "../../Graph.h"

// Forward
class ProofingView;
class IInputManager;
class ITemperatureController;

class ProofingController : public Screen {
public:
    explicit ProofingController(AppContext* ctx);
    void beginImpl() override;
    bool update(bool forceRedraw = false) override;

private:
    ProofingView* _view;
    IInputManager* _inputManager;
    AppContext* _ctx;
    time_t _startTime;
    time_t _lastTemperatureUpdate;
    time_t _lastGraphUpdate;
    time_t _previousDiffSeconds;
    Graph _temperatureGraph;
    ITemperatureController* _temperatureController;
};
