#include "PowerOffController.h"
#include "../views/PowerOffView.h"
#include "../../DebugUtils.h"
#include "SafePtr.h"
#include <esp_sleep.h>
#include <driver/gpio.h>
#include <Arduino.h>

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
            DEBUG_PRINTLN("Display cleared");
        }
    }
    
    // Configure button pin as wake-up source for ESP32-C3
    // ENCODER_SW is on GPIO10 as defined in main.cpp
    // GPIO10 is not in RTC domain on ESP32-C3, so we use light sleep instead of deep sleep
    // Light sleep still provides significant power savings and supports any GPIO
    esp_sleep_enable_gpio_wakeup();
    gpio_wakeup_enable(GPIO_NUM_10, GPIO_INTR_LOW_LEVEL);
    
    DEBUG_PRINTLN("Entering light sleep mode. Press button to wake.");
    delay(100);  // Allow time for serial output
    
    // Enter light sleep (wakes on button press)
    esp_light_sleep_start();
    
    // After waking from light sleep, we need to restart to reinitialize everything
    DEBUG_PRINTLN("Woke from sleep, restarting...");
    delay(100);
    ESP.restart();
}
