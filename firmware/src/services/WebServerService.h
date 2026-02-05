#pragma once
#include "IWebServerService.h"
#include <ESPAsyncWebServer.h>

// Forward declarations
struct AppContext;

namespace services {
    class WebServerService : public IWebServerService {
    public:
        explicit WebServerService(AppContext* ctx);
        ~WebServerService();
        void begin() override;
        void update() override;
        
        // WebSocket for real-time data updates
        void broadcastScreenState() override;
        
        // Notify web view of state changes (web is a VIEW)
        void notifyStateChange() override;

    private:
        AppContext* _ctx;
        AsyncWebServer* _server;  // Pointer to delay port 80 allocation
        AsyncWebSocket* _ws;      // WebSocket for display updates
        
        // State tracking to avoid redundant broadcasts
        struct LastBroadcastState {
            float temperature = -999.0f;
            ITemperatureController::Mode mode = ITemperatureController::OFF;
            String screenName;
            int proofingElapsed = -1;
            int coolingRemaining = -1;
            bool hasChanged() const { return true; } // Will be set properly
        };
        LastBroadcastState _lastBroadcast;
        
        void setupRoutes();
        void setupWebSocket();
        void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, 
                            AwsEventType type, void* arg, uint8_t* data, size_t len);
        
        void handleGetSettings(AsyncWebServerRequest* request);
        void handleSetMode(AsyncWebServerRequest* request);
        void handleSetSettings(AsyncWebServerRequest* request);
        void handleScheduleCooling(AsyncWebServerRequest* request);
        void handleGetRoot(AsyncWebServerRequest* request);
        
        // New virtual input endpoints
        void handleInjectButton(AsyncWebServerRequest* request);
        void handleInjectEncoder(AsyncWebServerRequest* request);
        
        // Quick action endpoints
        void handleProofNow(AsyncWebServerRequest* request);
        void handleProofAt(AsyncWebServerRequest* request, uint8_t* data, size_t len);
        void handleProofIn(AsyncWebServerRequest* request, uint8_t* data, size_t len);
        void handleStopOperation(AsyncWebServerRequest* request);
        
        String getWebPageHtml();
    };
}
