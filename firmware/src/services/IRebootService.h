#pragma once

namespace services {
    struct IRebootService {
        virtual ~IRebootService() {}
        virtual void reboot() = 0;
    };
}
