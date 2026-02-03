// Lightweight dependency container for wiring shared components and services
#pragma once
#include <stdint.h>
#include <driver/gpio.h>

class IDisplayManager;
class IInputManager;
class ITemperatureController;
struct ScreensManager;
// Forward declarations for view types
class AdjustValueView;
class AdjustTimeView;
class CoolingView;
class ProofingView;
class RebootView;
class WiFiResetView;
class DataDisplayView;
class ConfirmTimezoneView;
class PowerOffView;
namespace services { struct INetworkService; struct IStorage; struct IWebServerService; }

namespace services {
    struct IRebootService;
}

struct AppContext {
    IDisplayManager* display = nullptr;
    IInputManager* input = nullptr;
    ITemperatureController* tempController = nullptr;
    ScreensManager* screens = nullptr;
    services::IRebootService* rebootService = nullptr;
    services::INetworkService* networkService = nullptr;
    services::IStorage* storage = nullptr;
    services::IWebServerService* webServerService = nullptr;
    
    // Hardware configuration
    gpio_num_t encoderButtonPin = GPIO_NUM_NC;
    gpio_num_t heaterRelayPin = GPIO_NUM_NC;
    gpio_num_t coolerRelayPin = GPIO_NUM_NC;
    gpio_num_t proofingLedPin = GPIO_NUM_NC;
    gpio_num_t coolingLedPin = GPIO_NUM_NC;
    
    // Static view instances
    AdjustValueView* adjustValueView = nullptr;
    AdjustTimeView* adjustTimeView = nullptr;
    CoolingView* coolingView = nullptr;
    ProofingView* proofingView = nullptr;
    RebootView* rebootView = nullptr;
    WiFiResetView* wifiResetView = nullptr;
    DataDisplayView* dataDisplayView = nullptr;
    ConfirmTimezoneView* confirmTimezoneView = nullptr;
    PowerOffView* powerOffView = nullptr;
};
