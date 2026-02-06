#pragma once
#include <Arduino.h>

// Header-only templated ring buffer with compile-time size
template<uint8_t N>
class LineRingBuffer {
    static_assert(N > 0, "LineRingBuffer size must be > 0");
    static_assert(N <= 255, "LineRingBuffer size must fit in uint8_t");
public:
    void clear() {
        _startIndex = 0;
        _count = 0;
        for (uint8_t i = 0; i < N; ++i) {
            _lines[i].clear();
        }
    }

    void pushLine(const String& s) {
        if (_count < N) {
            const uint8_t insertIndex = (uint8_t)((_startIndex + _count) % N);
            _lines[insertIndex] = s;
            _count++;
        } else {
            _lines[_startIndex] = s;
            _startIndex = (uint8_t)((_startIndex + 1) % N);
        }
    }

    void setLastLine(const String& s) {
        if (_count == 0) {
            pushLine(s);
            return;
        }
        const uint8_t lastIndex = (uint8_t)((_startIndex + _count - 1) % N);
        _lines[lastIndex] = s;
    }

    void appendToLastLine(const char* s) {
        if (_count == 0) {
            pushLine(String(s));
            return;
        }
        const uint8_t lastIndex = (uint8_t)((_startIndex + _count - 1) % N);
        _lines[lastIndex] += s;
    }

    uint8_t count() const {
        return _count;
    }

    const String& lineAt(uint8_t i) const {
        uint8_t idx = (uint8_t)((_startIndex + i) % N);
        return _lines[idx];
    }

private:
    String _lines[N];
    uint8_t _startIndex = 0; // index of the oldest line
    uint8_t _count = 0;      // number of lines currently stored
};