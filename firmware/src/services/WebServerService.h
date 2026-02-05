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
        
        // Display mirroring via WebSocket
        void broadcastDisplayUpdate(const String& command);

    private:
        AppContext* _ctx;
        AsyncWebServer* _server;  // Pointer to delay port 80 allocation
        AsyncWebSocket* _ws;      // WebSocket for display mirroring
        
        void setupRoutes();
        void setupWebSocket();
        void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, 
                            AwsEventType type, void* arg, uint8_t* data, size_t len);
        
        void handleGetStatus(AsyncWebServerRequest* request);
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
