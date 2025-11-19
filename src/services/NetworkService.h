#pragma once
#include "INetworkService.h"

namespace services {
    struct NetworkService : public INetworkService {
        void autoConnect() override;
        void resetSettings() override;
        void configureNtp(const char* timezone, const char* server1, const char* server2) override;
        bool isTimeSyncReady(uint32_t threshold) override;
    };
}
