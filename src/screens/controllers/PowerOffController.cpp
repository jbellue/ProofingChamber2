#include "PowerOffController.h"
#include "../views/PowerOffView.h"
#include "../../DebugUtils.h"
#include "../../DisplayManager.h"
#include "SafePtr.h"
#include <esp_sleep.h>

PowerOffController::PowerOffController(AppContext* ctx)
    : BaseController(ctx), _view(nullptr), _onCancelButton(true) {}


void PowerOffController::beginImpl() {
    initializeInputManager();
    AppContext* ctx = getContext();
    if (ctx) {
        _view = ctx->powerOffView;
    }
    _onCancelButton = true;
    _view->start();
    _view->sendBuffer();
}

bool PowerOffController::update(bool shouldRedraw) {
    IInputManager* inputManager = getInputManager();
    const auto encoderDirection = inputManager->getEncoderDirection();
    if (encoderDirection != IInputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        _view->drawButtons(_onCancelButton);
        _view->sendBuffer();
    }
    if (inputManager->isButtonPressed()) {
        if (_onCancelButton) {
            return false;
        }
        performPowerOff();
        // Should not reach here after deep sleep
    }
    return true;
}

void PowerOffController::performPowerOff() {
    DEBUG_PRINTLN("PowerOffController: Powering off system");
    
    AppContext* ctx = getContext();
    if (ctx) {
        // Turn off temperature controller (relays)
        if (ctx->tempController) {
            ctx->tempController->setMode(ITemperatureController::OFF);
            DEBUG_PRINTLN("Temperature controller set to OFF");
        }
        
        // Clear and turn off display
        if (ctx->display) {
            ctx->display->clearBuffer();
            ctx->display->sendBuffer();
            
            // Access the U8G2 display to put it into power save mode
            DisplayManager* displayMgr = dynamic_cast<DisplayManager*>(ctx->display);
            if (displayMgr && displayMgr->getDisplay()) {
                displayMgr->getDisplay()->setPowerSave(1);  // 1 = power save on
                DEBUG_PRINTLN("Display power save enabled");
            }
        }
    }
    
    // Configure button pin as wake-up source
    // ENCODER_SW is on GPIO10 as defined in main.cpp - hardcoded here since 
    // it's a hardware constant and we need the gpio_num_t type for ESP sleep API
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_10, 0);  // 0 = wake on LOW (button pressed)
    
    DEBUG_PRINTLN("Entering deep sleep mode. Press button to wake.");
    delay(100);  // Allow time for serial output
    
    // Enter deep sleep
    esp_deep_sleep_start();
}
