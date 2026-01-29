#include <esp_sleep.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "PowerOffController.h"
#include "../views/PowerOffView.h"
#include "../../DebugUtils.h"
#include "ITemperatureController.h"

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

void gpioOff(const gpio_num_t pin) {
    // Drive the pin to a defined inactive state and hold it during deep sleep
    if (pin == GPIO_NUM_NC) {
        return;
    }
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    gpio_pullup_dis(pin);
    gpio_pulldown_dis(pin);
    gpio_hold_en(pin);
}

void PowerOffController::performPowerOff() {
    DEBUG_PRINTLN("PowerOffController: Powering off system");
    
    AppContext* ctx = getContext();
    if (ctx) {
        ctx->tempController->setMode(ITemperatureController::OFF);
        DEBUG_PRINTLN("Temperature controller set to OFF");
 
        ctx->display->clearBuffer();
        ctx->display->sendBuffer();
        DEBUG_PRINTLN("Display cleared");

        gpioOff(ctx->proofingLedPin);
        gpioOff(ctx->coolingLedPin);
        gpioOff(ctx->heaterRelayPin);
        gpioOff(ctx->coolerRelayPin);
    }
    
    // Configure button pin as wake-up source for ESP32-C3
    gpio_num_t buttonPin = ctx ? ctx->encoderButtonPin : GPIO_NUM_5; // Default to GPIO_NUM_5 if context unavailable
    esp_err_t err = esp_deep_sleep_enable_gpio_wakeup(1ULL << buttonPin, ESP_GPIO_WAKEUP_GPIO_LOW);
    if (err != ESP_OK) {
        DEBUG_PRINT("Failed to enable GPIO wakeup: ");
        DEBUG_PRINTLN(err);
    }
    
    // Keep held output levels during deep sleep to avoid floating pins
    gpio_deep_sleep_hold_en();

    DEBUG_PRINTLN("Entering deep sleep mode. Press button to wake.");
    vTaskDelay(pdMS_TO_TICKS(500));  // Allow time for serial output and button release
    
    esp_deep_sleep_start();
}
