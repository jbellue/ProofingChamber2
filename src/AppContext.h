// Lightweight dependency container for wiring shared components and services
#pragma once

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
class SetTimezoneView;
class WiFiResetView;
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
    
    // Static view instances
    AdjustValueView* adjustValueView = nullptr;
    AdjustTimeView* adjustTimeView = nullptr;
    CoolingView* coolingView = nullptr;
    ProofingView* proofingView = nullptr;
    RebootView* rebootView = nullptr;
    SetTimezoneView* setTimezoneView = nullptr;
    WiFiResetView* wifiResetView = nullptr;
};
