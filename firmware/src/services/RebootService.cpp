#include "RebootService.h"
#include <Arduino.h>

// Platform-specific reboot; wrapped so we can mock in tests
namespace services {
    void RebootService::reboot() {
        ESP.restart();
    }
}
