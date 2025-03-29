#ifndef PROOFING_SCREEN_H
#define PROOFING_SCREEN_H

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"

class ProofingScreen : public Screen {
public:
    ProofingScreen(DisplayManager* display, InputManager* inputManager);
    void beginImpl() override;
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    enum temperature_change_t {
        TEMPERATURE_LOWERING,
        TEMPERATURE_STABLE,
        TEMPERATURE_RISING,
        TEMPERATURE_UNKNOWN
    };
    DisplayManager* _display;
    InputManager* _inputManager;
    time_t _startTime;
    uint _previousDiffSeconds;
    float _previousTemp;
    float _currentTemp;
    temperature_change_t _temperatureChange;
    bool _isIconOn;

    void drawScreen();
    void drawTime();
};

#endif
