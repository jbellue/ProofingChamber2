#pragma once
#include "IWebServerService.h"
#include <ESPAsyncWebServer.h>

// Forward declarations
struct AppContext;

namespace services {
    class WebServerService : public IWebServerService {
    public:
        explicit WebServerService(AppContext* ctx);
        void begin() override;
        void update() override;

    private:
        AppContext* _ctx;
        AsyncWebServer _server;
        
        void setupRoutes();
        void handleGetStatus(AsyncWebServerRequest* request);
        void handleGetSettings(AsyncWebServerRequest* request);
        void handleSetMode(AsyncWebServerRequest* request);
        void handleSetSettings(AsyncWebServerRequest* request);
        void handleScheduleCooling(AsyncWebServerRequest* request);
        void handleGetRoot(AsyncWebServerRequest* request);
        
        String getWebPageHtml();
    };
}
