// Mock WiFiManager.h for unit testing
#pragma once

class WiFiManager {
public:
    WiFiManager() {}
    bool autoConnect(const char *apName) { return true; } // Mock success
};
