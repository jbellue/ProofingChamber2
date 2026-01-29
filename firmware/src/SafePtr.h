#pragma once

#include "IInputManager.h"
#include "ITemperatureController.h"
#include "IDisplayManager.h"
#include "services/INetworkService.h"
#include "services/IStorage.h"
#include "services/IRebootService.h"
#include "NullInputManager.h"
#include "NullTemperatureController.h"
#include "NullDisplayManager.h"
#include "NullRebootService.h"
#include "NullNetworkService.h"
#include "NullStorage.h"

/**
 * Helper class for safe pointer resolution using Null Object Pattern
 * 
 * Usage Example:
 * 
 *   IInputManager* input = SafePtr::resolve(_ctx->input);
 *   float temp = input->getTemperature(); // Safe! Never crashes
 * 
 * If _ctx->input is nullptr, returns NullInputManager that
 * provides safe default values instead of crashing.
 */
class SafePtr {
public:
    static IInputManager* resolve(IInputManager* ptr) {
        if (ptr == nullptr) {
            return &NullInputManager::getInstance();
        }
        return ptr;
    }

    static ITemperatureController* resolve(ITemperatureController* ptr) {
        if (ptr == nullptr) {
            return &NullTemperatureController::getInstance();
        }
        return ptr;
    }

    static IDisplayManager* resolve(IDisplayManager* ptr) {
        if (ptr == nullptr) {
            return &NullDisplayManager::getInstance();
        }
        return ptr;
    }

    static services::INetworkService* resolve(services::INetworkService* ptr) {
        if (ptr == nullptr) {
            return &services::NullNetworkService::getInstance();
        }
        return ptr;
    }

    static services::IStorage* resolve(services::IStorage* ptr) {
        if (ptr == nullptr) {
            return &services::NullStorage::getInstance();
        }
        return ptr;
    }

    template<typename T>
    static bool isNull(T* ptr) {
        return ptr == nullptr;
    }
};
