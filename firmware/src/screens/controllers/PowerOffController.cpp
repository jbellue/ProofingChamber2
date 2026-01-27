#include <esp_sleep.h>
#include "PowerOffController.h"
#include "../views/PowerOffView.h"
#include "../../DebugUtils.h"
#include "SafePtr.h"

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
    uint8_t buttonPin = ctx ? ctx->encoderButtonPin : 5; // Default to 5 if context unavailable
    esp_err_t err = esp_deep_sleep_enable_gpio_wakeup(1ULL << buttonPin, ESP_GPIO_WAKEUP_GPIO_LOW);
    if (err != ESP_OK) {
        DEBUG_PRINT("Failed to enable GPIO wakeup: ");
        DEBUG_PRINTLN(err);
    }
    
    DEBUG_PRINTLN("Entering deep sleep mode. Press button to wake.");
    delay(500);  // Allow time for serial output and button release
    
    esp_deep_sleep_start();
}
