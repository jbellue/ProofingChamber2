#include "NetworkService.h"
#include <WiFiManager.h>
#include <WiFi.h>
#include <time.h>

namespace services {

bool NetworkService::autoConnect(const char* portalSsid,
                                 std::function<void(const char* apName)> onPortalStarted) {
    WiFiManager wifiManager;
    // Make reconnection more reliable and give the user enough time to enter credentials
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    wifiManager.setCleanConnect(true);          // forget any half-open connection attempts
    wifiManager.setConnectTimeout(20);          // seconds to wait for WiFi association
    wifiManager.setConfigPortalTimeout(60);     // keep portal up long enough for user input without hanging too long
    wifiManager.setWiFiAutoReconnect(true);
    wifiManager.setBreakAfterConfig(true);      // exit once credentials are saved (even if connect fails now)
    if (onPortalStarted) {
        wifiManager.setAPCallback([onPortalStarted](WiFiManager* wm) {
            String apName = wm ? wm->getConfigPortalSSID() : String("ConfigPortal");
            IPAddress apIp = WiFi.softAPIP();
            onPortalStarted(apName.c_str());
        });
    }
    if (portalSsid && portalSsid[0] != '\0') {
        return wifiManager.autoConnect(portalSsid);
    }
    return wifiManager.autoConnect();
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
