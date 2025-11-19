#pragma once
#include <stdint.h>

namespace services {
    struct INetworkService {
        virtual ~INetworkService() {}
        virtual void autoConnect() = 0;
        virtual void resetSettings() = 0;
        virtual void configureNtp(const char* timezone, const char* server1, const char* server2) = 0;
        virtual bool isTimeSyncReady(uint32_t threshold) = 0;
    };
}
