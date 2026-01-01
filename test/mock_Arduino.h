// Mock Arduino.h for native testing
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <map>

// Mock Arduino types and constants
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

typedef bool boolean;
typedef uint8_t byte;

// Mock pin states for testing
namespace arduino_mock {
    extern std::map<uint8_t, uint8_t> pinModes;
    extern std::map<uint8_t, uint8_t> pinStates;
}

// Mock Arduino functions with implementations
inline void pinMode(uint8_t pin, uint8_t mode) {
    arduino_mock::pinModes[pin] = mode;
}

inline void digitalWrite(uint8_t pin, uint8_t value) {
    arduino_mock::pinStates[pin] = value;
}

inline int digitalRead(uint8_t pin) {
    return arduino_mock::pinStates[pin];
}

inline void delay(unsigned long) {}
inline unsigned long millis() { return 0; }
inline unsigned long micros() { return 0; }

// Math functions
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

