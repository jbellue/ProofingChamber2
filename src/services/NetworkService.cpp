#include "NetworkService.h"
#include <WiFiManager.h>
#include <time.h>

namespace services {

void NetworkService::autoConnect() {
    WiFiManager wifiManager;
    wifiManager.autoConnect();
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
