#pragma once
#include "INetworkService.h"

// Forward declaration to avoid including WiFiManager.h in header
class WiFiManager;

namespace services {
    struct NetworkService : public INetworkService {
        NetworkService();
        ~NetworkService();
        
        bool autoConnect(const char* portalSsid = nullptr,
                 std::function<void(const char* apName)> onPortalStarted = nullptr) override;
        void resetSettings() override;
        void configureNtp(const char* timezone, const char* server1, const char* server2) override;
        bool isTimeSyncReady(uint32_t threshold) override;
        
    private:
        WiFiManager* _wifiManager;  // Keep WiFiManager alive as member variable
    };
}
