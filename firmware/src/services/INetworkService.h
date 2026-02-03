#pragma once
#include <stdint.h>
#include <functional>

namespace services {
    struct INetworkService {
        virtual ~INetworkService() {}
        virtual bool autoConnect(const char* portalSsid = nullptr,
                     std::function<void(const char* apName)> onPortalStarted = nullptr) = 0;
        virtual bool startConfigPortal(const char* portalSsid = nullptr,
                     std::function<void(const char* apName)> onPortalStarted = nullptr) = 0;
        virtual void resetSettings() = 0;
        virtual void configureNtp(const char* timezone, const char* server1, const char* server2) = 0;
        virtual bool isTimeSyncReady(uint32_t threshold) = 0;
    };
}
