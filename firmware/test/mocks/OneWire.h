// Mock OneWire.h for unit testing
#pragma once

#include <stdint.h>

class OneWire {
public:
    OneWire(uint8_t pin) {}
    void reset_search() {}
    uint8_t search(uint8_t *newAddr) { return 0; }
    uint8_t crc8(const uint8_t *addr, uint8_t len) { return 0; }
};
