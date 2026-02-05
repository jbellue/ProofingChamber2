#pragma once

namespace services {
    struct IWebServerService {
        virtual ~IWebServerService() = default;
        virtual void begin() = 0;
        virtual void update() = 0;
        virtual void notifyStateChange() = 0;
    };
}
