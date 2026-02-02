#pragma once

#include "../BaseController.h"
#include "../../AppContextDecl.h"
#include "../../Graph.h"

// Forward
class ProofingView;
class IInputManager;
class ITemperatureController;

class ProofingController : public BaseController {
public:
    explicit ProofingController(AppContext* ctx);
    void beginImpl() override;
    bool update(bool forceRedraw = false) override;

private:
    ProofingView* _view;
    time_t _startTime;
    time_t _lastTemperatureUpdate;
    time_t _lastGraphUpdate;
    time_t _previousDiffSeconds;
    Graph _temperatureGraph;
    ITemperatureController* _temperatureController;
};
