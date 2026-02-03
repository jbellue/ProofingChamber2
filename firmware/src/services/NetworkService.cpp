#include "NetworkService.h"
#include <WiFiManager.h>
#include <WiFi.h>
#include <time.h>
#include <ESPmDNS.h>
#include "../DebugUtils.h"

namespace services {

bool NetworkService::autoConnect(const char* portalSsid,
                                 std::function<void(const char* apName)> onPortalStarted) {
    DEBUG_PRINTLN("=== WiFi Connection Starting ===");
    
    // CRITICAL: Ensure clean WiFi state before starting
    // This is essential for WiFiManager to work correctly
    DEBUG_PRINTLN("Stopping any existing WiFi...");
    WiFi.disconnect(true);  // Disconnect and erase credentials from RAM
    WiFi.mode(WIFI_OFF);    // Turn off WiFi completely
    delay(100);             // Give WiFi time to fully shut down
    
    DEBUG_PRINTLN("Setting WiFi mode to STA...");
    WiFi.mode(WIFI_STA);    // Set to station mode (required for WiFiManager)
    delay(100);             // Give WiFi time to initialize
    
    DEBUG_PRINTLN("Creating WiFiManager instance...");
    WiFiManager wifiManager;
    
    // Configure WiFiManager before autoConnect
    // WiFiManager handles ALL WiFi persistence and reconnection internally
    // We should NOT call WiFi.persistent() or WiFi.setAutoReconnect() ourselves
    DEBUG_PRINTLN("Configuring WiFiManager...");
    wifiManager.setCleanConnect(true);          // forget any half-open connection attempts
    wifiManager.setConnectTimeout(20);          // seconds to wait for WiFi association
    wifiManager.setConfigPortalTimeout(0);      // 0 = NO TIMEOUT - portal stays open until configured
    wifiManager.setWiFiAutoReconnect(true);     // WiFiManager will set auto-reconnect
    wifiManager.setBreakAfterConfig(true);      // exit once credentials are saved
    wifiManager.setSaveConfigCallback([]() {
        DEBUG_PRINTLN("✓ WiFiManager saved credentials to NVS");
    });
    
    // Enable debug output to help diagnose issues
    wifiManager.setDebugOutput(true);
    DEBUG_PRINTLN("WiFiManager debug output enabled");
    
    if (onPortalStarted) {
        wifiManager.setAPCallback([onPortalStarted](WiFiManager* wm) {
            // CRITICAL FIX: When portal starts after failed connection attempts,
            // the WiFi radio may be in a bad state (still in STA mode or transitioning).
            // We must explicitly reset to AP+STA mode for the AP to be visible.
            DEBUG_PRINTLN("Portal starting - ensuring WiFi is in AP+STA mode...");
            WiFi.mode(WIFI_OFF);
            delay(100);  // Let radio fully power down
            WiFi.mode(WIFI_AP_STA);  // Explicitly set to AP+STA mode
            delay(200);  // Give extra time for AP to become visible
            
            String apName = wm ? wm->getConfigPortalSSID() : String("ConfigPortal");
            IPAddress apIp = WiFi.softAPIP();
            DEBUG_PRINTLN("╔════════════════════════════════════════╗");
            DEBUG_PRINTLN("║   CAPTIVE PORTAL STARTED!              ║");
            DEBUG_PRINTLN("╚════════════════════════════════════════╝");
            DEBUG_PRINT("  AP Name: ");
            DEBUG_PRINTLN(apName.c_str());
            DEBUG_PRINT("  AP IP: ");
            DEBUG_PRINTLN(apIp.toString().c_str());
            DEBUG_PRINT("  WiFi Mode: ");
            DEBUG_PRINTLN(WiFi.getMode() == WIFI_AP_STA ? "AP+STA (correct)" : "WRONG MODE!");
            DEBUG_PRINTLN("  Connect to this network and configure WiFi");
            DEBUG_PRINTLN("  The network should now be visible on your device");
            onPortalStarted(apName.c_str());
        });
    }
    
    // Try to connect; if no credentials exist, portal will start automatically
    DEBUG_PRINTLN("Calling WiFiManager.autoConnect()...");
    DEBUG_PRINT("  Portal SSID: ");
    DEBUG_PRINTLN(portalSsid ? portalSsid : "(default)");
    
    bool connected = false;
    if (portalSsid && portalSsid[0] != '\0') {
        connected = wifiManager.autoConnect(portalSsid);
    } else {
        connected = wifiManager.autoConnect();
    }
    
    if (connected) {
        DEBUG_PRINTLN("╔════════════════════════════════════════╗");
        DEBUG_PRINTLN("║   WiFi CONNECTED SUCCESSFULLY!         ║");
        DEBUG_PRINTLN("╚════════════════════════════════════════╝");
        DEBUG_PRINT("  SSID: ");
        DEBUG_PRINTLN(WiFi.SSID().c_str());
        DEBUG_PRINT("  IP Address: ");
        DEBUG_PRINTLN(WiFi.localIP().toString().c_str());
        DEBUG_PRINT("  Signal Strength: ");
        DEBUG_PRINT(WiFi.RSSI());
        DEBUG_PRINTLN(" dBm");
        
        // Set up mDNS so users can access via http://proofingchamber.local
        DEBUG_PRINTLN("Starting mDNS responder...");
        if (MDNS.begin("proofingchamber")) {
            DEBUG_PRINTLN("✓ mDNS responder started: proofingchamber.local");
            MDNS.addService("http", "tcp", 80);
        } else {
            DEBUG_PRINTLN("✗ Error setting up mDNS responder!");
        }
        DEBUG_PRINTLN("========================================");
    } else {
        DEBUG_PRINTLN("╔════════════════════════════════════════╗");
        DEBUG_PRINTLN("║   WiFi CONNECTION FAILED               ║");
        DEBUG_PRINTLN("╚════════════════════════════════════════╝");
        DEBUG_PRINTLN("  Possible reasons:");
        DEBUG_PRINTLN("  - Portal timeout (user didn't configure)");
        DEBUG_PRINTLN("  - Connection timeout (wrong credentials)");
        DEBUG_PRINTLN("  - Hardware issue");
        DEBUG_PRINTLN("========================================");
    }
    
    // NOTE: We do NOT call WiFi.persistent(true) or WiFi.setAutoReconnect(true) here
    // WiFiManager already handles persistence and auto-reconnect internally
    // Adding extra calls can cause conflicts and unreliable behavior
    
    return connected;
}

bool NetworkService::startConfigPortal(const char* portalSsid,
                                       std::function<void(const char* apName)> onPortalStarted) {
    DEBUG_PRINTLN("=== FORCED CONFIG PORTAL MODE ===");
    DEBUG_PRINTLN("This will ALWAYS start the captive portal,");
    DEBUG_PRINTLN("even if WiFi credentials exist.");
    
    // CRITICAL: Ensure clean WiFi state before starting
    DEBUG_PRINTLN("Stopping any existing WiFi...");
    WiFi.disconnect(true);  // Disconnect and erase credentials from RAM
    WiFi.mode(WIFI_OFF);    // Turn off WiFi completely
    delay(100);             // Give WiFi time to fully shut down
    
    DEBUG_PRINTLN("Setting WiFi mode to AP+STA...");
    WiFi.mode(WIFI_AP_STA); // Set to AP+STA mode for portal
    delay(100);             // Give WiFi time to initialize
    
    DEBUG_PRINTLN("Creating WiFiManager instance...");
    WiFiManager wifiManager;
    
    // Configure WiFiManager
    DEBUG_PRINTLN("Configuring WiFiManager...");
    wifiManager.setConfigPortalTimeout(0);      // No timeout - stay open until configured
    wifiManager.setBreakAfterConfig(true);      // exit once credentials are saved
    wifiManager.setSaveConfigCallback([]() {
        DEBUG_PRINTLN("✓ WiFiManager saved credentials to NVS");
    });
    
    // Enable debug output
    wifiManager.setDebugOutput(true);
    DEBUG_PRINTLN("WiFiManager debug output enabled");
    
    if (onPortalStarted) {
        wifiManager.setAPCallback([onPortalStarted](WiFiManager* wm) {
            // CRITICAL: Ensure WiFi is in AP+STA mode when forced portal starts
            DEBUG_PRINTLN("Forced portal starting - ensuring WiFi is in AP+STA mode...");
            WiFi.mode(WIFI_OFF);
            delay(100);
            WiFi.mode(WIFI_AP_STA);
            delay(200);
            
            String apName = wm ? wm->getConfigPortalSSID() : String("ConfigPortal");
            IPAddress apIp = WiFi.softAPIP();
            DEBUG_PRINTLN("╔════════════════════════════════════════╗");
            DEBUG_PRINTLN("║   CAPTIVE PORTAL STARTED (FORCED)!     ║");
            DEBUG_PRINTLN("╚════════════════════════════════════════╝");
            DEBUG_PRINT("  AP Name: ");
            DEBUG_PRINTLN(apName.c_str());
            DEBUG_PRINT("  AP IP: ");
            DEBUG_PRINTLN(apIp.toString().c_str());
            DEBUG_PRINTLN("  Connect to this network and configure WiFi");
            DEBUG_PRINTLN("  Portal will stay open until configured");
            onPortalStarted(apName.c_str());
        });
    }
    
    // Force portal mode - will NOT try saved credentials
    DEBUG_PRINTLN("Starting FORCED config portal...");
    DEBUG_PRINT("  Portal SSID: ");
    DEBUG_PRINTLN(portalSsid ? portalSsid : "ESP_AP");
    
    bool connected = false;
    if (portalSsid && portalSsid[0] != '\0') {
        connected = wifiManager.startConfigPortal(portalSsid);
    } else {
        connected = wifiManager.startConfigPortal();
    }
    
    if (connected) {
        DEBUG_PRINTLN("╔════════════════════════════════════════╗");
        DEBUG_PRINTLN("║   WiFi CONFIGURED SUCCESSFULLY!        ║");
        DEBUG_PRINTLN("╚════════════════════════════════════════╝");
        DEBUG_PRINT("  SSID: ");
        DEBUG_PRINTLN(WiFi.SSID().c_str());
        DEBUG_PRINT("  IP Address: ");
        DEBUG_PRINTLN(WiFi.localIP().toString().c_str());
        
        // Set up mDNS
        if (MDNS.begin("proofingchamber")) {
            DEBUG_PRINTLN("✓ mDNS responder started: proofingchamber.local");
            MDNS.addService("http", "tcp", 80);
        }
        DEBUG_PRINTLN("========================================");
    } else {
        DEBUG_PRINTLN("╔════════════════════════════════════════╗");
        DEBUG_PRINTLN("║   PORTAL EXITED WITHOUT CONFIGURATION  ║");
        DEBUG_PRINTLN("╚════════════════════════════════════════╝");
        DEBUG_PRINTLN("========================================");
    }
    
    return connected;
}

void NetworkService::resetSettings() {
    DEBUG_PRINTLN("=== RESETTING WiFi SETTINGS ===");
    DEBUG_PRINTLN("This will erase all saved WiFi credentials");
    
    // Stop WiFi completely first
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
    
    // Reset WiFiManager settings (erases NVS)
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    
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
