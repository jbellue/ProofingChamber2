#ifndef PROOFING_SCREEN_H
#define PROOFING_SCREEN_H

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"

class ProofingScreen : public Screen {
public:
    ProofingScreen(DisplayManager* display, InputManager* inputManager);
    void beginImpl() override {};
    void begin(const char* startTime, int initialTemp, bool isRising, bool isIconOn);
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* display;
    InputManager* inputManager;
    const char* startTime;
    int currentTemp;
    bool isRising;
    bool isIconOn;

    void drawScreen();
    void beginImpl(const char* startTime, int initialTemp, bool isRising, bool isIconOn);
};

#endif
