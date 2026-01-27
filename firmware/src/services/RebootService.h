#pragma once
#include "IRebootService.h"

namespace services {
    struct RebootService : public IRebootService {
        void reboot() override;
    };
}
