#pragma once
#include "INetworkService.h"

namespace services {
    struct NetworkService : public INetworkService {
        bool autoConnect(const char* portalSsid = nullptr,
                 std::function<void(const char* apName)> onPortalStarted = nullptr) override;
        bool startConfigPortal(const char* portalSsid = nullptr,
                 std::function<void(const char* apName)> onPortalStarted = nullptr) override;
        void resetSettings() override;
        void configureNtp(const char* timezone, const char* server1, const char* server2) override;
        bool isTimeSyncReady(uint32_t threshold) override;
    };
}
