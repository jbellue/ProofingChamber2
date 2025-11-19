// Lightweight dependency container for wiring shared components and services
#pragma once

struct DisplayManager;
struct InputManager;
struct ScreensManager;
struct TemperatureController;
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
};
