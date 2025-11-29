// Lightweight dependency container for wiring shared components and services
#pragma once

struct DisplayManager;
struct InputManager;
struct ScreensManager;
struct TemperatureController;
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
    DisplayManager* display = nullptr;
    InputManager* input = nullptr;
    ScreensManager* screens = nullptr;
    TemperatureController* tempController = nullptr;
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
