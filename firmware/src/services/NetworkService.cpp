#include "NetworkService.h"
#include <WiFiManager.h>
#include <WiFi.h>
#include <time.h>

namespace services {

bool NetworkService::autoConnect(const char* portalSsid,
                                 std::function<void(const char* apName)> onPortalStarted) {
    WiFiManager wifiManager;
    
    // Configure WiFiManager before autoConnect
    // Let WiFiManager handle WiFi settings internally to avoid conflicts
    wifiManager.setCleanConnect(true);          // forget any half-open connection attempts
    wifiManager.setConnectTimeout(20);          // seconds to wait for WiFi association
    wifiManager.setConfigPortalTimeout(180);    // 3 minutes for portal - was too short at 60s
    wifiManager.setWiFiAutoReconnect(true);     // enable auto-reconnect after successful connection
    wifiManager.setBreakAfterConfig(true);      // exit once credentials are saved
    
    // IMPORTANT: Don't set WiFi.persistent() or WiFi.setAutoReconnect() here
    // Let WiFiManager control WiFi state to ensure captive portal can start properly
    
    if (onPortalStarted) {
        wifiManager.setAPCallback([onPortalStarted](WiFiManager* wm) {
            String apName = wm ? wm->getConfigPortalSSID() : String("ConfigPortal");
            IPAddress apIp = WiFi.softAPIP();
            onPortalStarted(apName.c_str());
        });
    }
    
    // Try to connect; if no credentials exist, portal will start automatically
    bool connected = false;
    if (portalSsid && portalSsid[0] != '\0') {
        connected = wifiManager.autoConnect(portalSsid);
    } else {
        connected = wifiManager.autoConnect();
    }
    
    // After successful connection, enable persistence for future boots
    if (connected) {
        WiFi.setAutoReconnect(true);
        WiFi.persistent(true);
    }
    
    return connected;
}

void NetworkService::resetSettings() {
    WiFiManager wifiManager;
    wifiManager.resetSettings();
}

void NetworkService::configureNtp(const char* timezone, const char* server1, const char* server2) {
    configTzTime(timezone, server1, server2);
}

bool NetworkService::isTimeSyncReady(uint32_t threshold) {
    return time(nullptr) >= (time_t)threshold;
}

} // namespace services
