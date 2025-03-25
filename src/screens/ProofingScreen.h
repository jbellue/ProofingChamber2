#ifndef PROOFING_SCREEN_H
#define PROOFING_SCREEN_H

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"

class ProofingScreen : public Screen {
public:
    ProofingScreen(DisplayManager* display, InputManager* inputManager);
    void beginImpl() override {};
    void begin(tm* startTime);
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    time_t _startTime;
    uint _previousDiffSeconds;
    int _currentTemp;
    bool _isRising;
    bool _isIconOn;

    void drawScreen();
    void drawTime();
    void beginImpl(tm* startTime);
};

#endif
