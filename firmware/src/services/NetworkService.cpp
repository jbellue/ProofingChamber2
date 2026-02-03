#include "NetworkService.h"
#include <WiFiManager.h>
#include <WiFi.h>
#include <time.h>
#include <ESPmDNS.h>
#include "../DebugUtils.h"

namespace services {

bool NetworkService::autoConnect(const char* portalSsid,
                                 std::function<void(const char* apName)> onPortalStarted) {
    WiFiManager wifiManager;
    
    // Configure WiFiManager before autoConnect
    // WiFiManager handles ALL WiFi persistence and reconnection internally
    // We should NOT call WiFi.persistent() or WiFi.setAutoReconnect() ourselves
    wifiManager.setCleanConnect(true);          // forget any half-open connection attempts
    wifiManager.setConnectTimeout(20);          // seconds to wait for WiFi association
    wifiManager.setConfigPortalTimeout(180);    // 3 minutes for portal
    wifiManager.setWiFiAutoReconnect(true);     // WiFiManager will set auto-reconnect
    wifiManager.setBreakAfterConfig(true);      // exit once credentials are saved
    wifiManager.setSaveConfigCallback([]() {
        DEBUG_PRINTLN("WiFiManager saved credentials");
    });
    
    // Enable debug output to help diagnose issues
    wifiManager.setDebugOutput(true);
    
    if (onPortalStarted) {
        wifiManager.setAPCallback([onPortalStarted](WiFiManager* wm) {
            String apName = wm ? wm->getConfigPortalSSID() : String("ConfigPortal");
            IPAddress apIp = WiFi.softAPIP();
            DEBUG_PRINT("Captive portal started: ");
            DEBUG_PRINTLN(apName.c_str());
            onPortalStarted(apName.c_str());
        });
    }
    
    // Try to connect; if no credentials exist, portal will start automatically
    DEBUG_PRINTLN("Starting WiFi connection attempt...");
    bool connected = false;
    if (portalSsid && portalSsid[0] != '\0') {
        connected = wifiManager.autoConnect(portalSsid);
    } else {
        connected = wifiManager.autoConnect();
    }
    
    if (connected) {
        DEBUG_PRINT("WiFi connected! IP: ");
        DEBUG_PRINTLN(WiFi.localIP().toString().c_str());
        
        // Set up mDNS so users can access via http://proofingchamber.local
        if (MDNS.begin("proofingchamber")) {
            DEBUG_PRINTLN("mDNS responder started: proofingchamber.local");
            MDNS.addService("http", "tcp", 80);
        } else {
            DEBUG_PRINTLN("Error setting up mDNS responder!");
        }
    } else {
        DEBUG_PRINTLN("WiFi connection failed");
    }
    
    // NOTE: We do NOT call WiFi.persistent(true) or WiFi.setAutoReconnect(true) here
    // WiFiManager already handles persistence and auto-reconnect internally
    // Adding extra calls can cause conflicts and unreliable behavior
    
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
