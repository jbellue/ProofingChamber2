#pragma once
#include "services/IRebootService.h"

namespace services {
class NullRebootService : public IRebootService {
public:
    static NullRebootService& getInstance() {
        static NullRebootService instance;
        return instance;
    }
    void reboot() override {}
private:
    NullRebootService() = default;
    NullRebootService(const NullRebootService&) = delete;
    NullRebootService& operator=(const NullRebootService&) = delete;
};
} // namespace services