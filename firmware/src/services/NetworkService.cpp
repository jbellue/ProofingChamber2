#include "NetworkService.h"
#include <WiFiManager.h>
#include <WiFi.h>
#include <time.h>
#include <ESPmDNS.h>
#include "../DebugUtils.h"

namespace services {

NetworkService::NetworkService() : _wifiManager(nullptr) {
    DEBUG_PRINTLN("NetworkService created");
}

NetworkService::~NetworkService() {
    DEBUG_PRINTLN("NetworkService destroyed - cleaning up WiFiManager");
    if (_wifiManager) {
        delete _wifiManager;
        _wifiManager = nullptr;
    }
}

bool NetworkService::autoConnect(const char* portalSsid,
                                 std::function<void(const char* apName)> onPortalStarted) {   
    DEBUG_PRINTLN("\n🔄 Resetting WiFi to clean state...");
    WiFi.mode(WIFI_OFF);    // Turn off WiFi completely (preserves credentials)
    delay(100);             // Let WiFi power down
    
    DEBUG_PRINTLN("  Setting WiFi mode to STA...");
    WiFi.mode(WIFI_STA);    // Set to station mode (required for WiFiManager)
    delay(100);             // Let mode transition complete
    DEBUG_PRINT("  Current WiFi mode: ");
    DEBUG_PRINTLN(String(WiFi.getMode()).c_str());
    
    DEBUG_PRINTLN("\n📡 Creating WiFiManager instance...");
    if (!_wifiManager) {
        _wifiManager = new WiFiManager();
        DEBUG_PRINTLN("  ✓ New WiFiManager instance created");
    } else {
        DEBUG_PRINTLN("  ✓ Reusing existing WiFiManager instance");
    }
    
    // Configure WiFiManager before autoConnect
    // WiFiManager handles ALL WiFi persistence and reconnection internally
    // We should NOT call WiFi.persistent() or WiFi.setAutoReconnect() ourselves
    DEBUG_PRINTLN("Configuring WiFiManager...");
    _wifiManager->setCleanConnect(true);          // forget any half-open connection attempts
    _wifiManager->setConnectTimeout(20);          // seconds to wait for WiFi association
    _wifiManager->setConfigPortalTimeout(0);      // 0 = NO TIMEOUT - portal stays open until configured
    _wifiManager->setWiFiAutoReconnect(true);     // WiFiManager will set auto-reconnect
    _wifiManager->setBreakAfterConfig(true);      // exit once credentials are saved
    _wifiManager->setSaveConfigCallback([]() {
        DEBUG_PRINTLN("✓ WiFiManager saved credentials to NVS");
    });
    
    // Enable debug output to help diagnose issues
    _wifiManager->setDebugOutput(true);
    DEBUG_PRINTLN("WiFiManager debug output enabled");
    
    if (onPortalStarted) {
        _wifiManager->setAPCallback([onPortalStarted](WiFiManager* wm) {
            // When portal starts after failed connection attempts,
            // ensure WiFi is in correct AP+STA mode
            DEBUG_PRINTLN("Portal starting");

            WiFi.mode(WIFI_OFF);
            delay(100);  // Let radio power down
            
            WiFi.mode(WIFI_AP_STA);
            delay(200);  // Let mode transition complete
            
            String apName = wm ? wm->getConfigPortalSSID() : String("ConfigPortal");
            IPAddress apIp = WiFi.softAPIP();
            wifi_mode_t mode = WiFi.getMode();
            
            onPortalStarted(apName.c_str());
        });
    }
    
    // Try to connect; if no credentials exist, portal will start automatically
    DEBUG_PRINTLN("Calling WiFiManager.autoConnect()...");
    DEBUG_PRINT("  Portal SSID: ");
    DEBUG_PRINTLN(portalSsid ? portalSsid : "(default)");
    
    bool connected = false;
    if (portalSsid && portalSsid[0] != '\0') {
        connected = _wifiManager->autoConnect(portalSsid);
    } else {
        connected = _wifiManager->autoConnect();
    }
    
    if (connected) {       
        // Set up mDNS so users can access via http://proofi.local
        DEBUG_PRINTLN("Starting mDNS responder...");
        if (MDNS.begin("proofi")) {
            DEBUG_PRINTLN("✓ mDNS responder started: proofi.local");
            MDNS.addService("http", "tcp", 80);
        } else {
            DEBUG_PRINTLN("✗ Error setting up mDNS responder!");
        }
    } else {
        DEBUG_PRINTLN("WiFi CONNECTION FAILED");
        DEBUG_PRINTLN("  Possible reasons:");
        DEBUG_PRINTLN("  - Portal timeout (user didn't configure)");
        DEBUG_PRINTLN("  - Connection timeout (wrong credentials)");
        DEBUG_PRINTLN("  - Hardware issue");
    }
    
    return connected;
}

void NetworkService::resetSettings() {
    // Stop WiFi completely first
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
    
    // Reset WiFiManager settings (erases NVS)
    // Use member instance if available, otherwise create temporary one
    if (_wifiManager) {
        _wifiManager->resetSettings();
        DEBUG_PRINTLN("  Used existing WiFiManager instance");
    } else {
        WiFiManager tempManager;
        tempManager.resetSettings();
        DEBUG_PRINTLN("  Used temporary WiFiManager instance");
    }
    
    DEBUG_PRINTLN("✓ WiFi settings reset complete");
    DEBUG_PRINTLN("  All credentials erased from NVS");
    DEBUG_PRINTLN("  Next boot will show captive portal");
    DEBUG_PRINTLN("========================================");
}

void NetworkService::configureNtp(const char* timezone, const char* server1, const char* server2) {
    configTzTime(timezone, server1, server2);
}

bool NetworkService::isTimeSyncReady(uint32_t threshold) {
    return time(nullptr) >= (time_t)threshold;
}

} // namespace services
