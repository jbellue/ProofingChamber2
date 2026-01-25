// Lightweight dependency container for wiring shared components and services
#pragma once
#include <stdint.h>

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
namespace services { struct INetworkService; struct IStorage; }

namespace services {
    struct IRebootService;
}

struct AppContext {
    IDisplayManager* display = nullptr;
    IInputManager* input = nullptr;
    ScreensManager* screens = nullptr;
    ITemperatureController* tempController = nullptr;
    services::IRebootService* rebootService = nullptr;
    services::INetworkService* networkService = nullptr;
    services::IStorage* storage = nullptr;
    
    // Hardware configuration
    uint8_t encoderButtonPin = 0;
    
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
