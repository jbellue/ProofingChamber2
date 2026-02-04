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
    
    const char* getScreenName() const override { return "Proofing"; }
    
    // Getters for web interface
    time_t getStartTime() const { return _startTime; }
    bool isActive() const { return _startTime != 0; }
    
    // Direct API methods for web interface
    void startProofing();
    void stopProofing();

private:
    ProofingView* _view;
    time_t _startTime;
    time_t _lastTemperatureUpdate;
    time_t _lastGraphUpdate;
    time_t _previousDiffSeconds;
    Graph _temperatureGraph;
    ITemperatureController* _temperatureController;
};
