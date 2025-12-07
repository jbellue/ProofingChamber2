#pragma once

#include "services/INetworkService.h"

// Null Object Pattern: Safe do-nothing implementation
namespace services {
class NullNetworkService : public INetworkService {
public:
    static NullNetworkService& getInstance() {
        static NullNetworkService instance;
        return instance;
    }

    bool autoConnect(const char* portalSsid = nullptr,
                     std::function<void(const char* apName)> onPortalStarted = nullptr) override {
        (void)portalSsid;
        (void)onPortalStarted;
        return false;
    }

    void resetSettings() override {}

    void configureNtp(const char* timezone, const char* server1, const char* server2) override {
        (void)timezone;
        (void)server1;
        (void)server2;
    }

    bool isTimeSyncReady(uint32_t threshold) override {
        (void)threshold;
        return false;
    }
private:
    NullNetworkService() = default;
    NullNetworkService(const NullNetworkService&) = delete;
    NullNetworkService& operator=(const NullNetworkService&) = delete;
};
} // namespace services
