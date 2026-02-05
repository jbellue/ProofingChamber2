// Mock Arduino.cpp implementation
#include "Arduino.h"
#include <chrono>
#include <thread>
#include <map>

// Global state for mocking
static std::map<uint8_t, uint8_t> pinModes;
static std::map<uint8_t, uint8_t> pinValues;
static auto startTime = std::chrono::steady_clock::now();

void pinMode(uint8_t pin, uint8_t mode) {
    pinModes[pin] = mode;
}

void digitalWrite(uint8_t pin, uint8_t val) {
    pinValues[pin] = val;
}

int digitalRead(uint8_t pin) {
    return pinValues.count(pin) ? pinValues[pin] : LOW;
}

int analogRead(uint8_t pin) {
    return 0; // Mock value
}

void analogWrite(uint8_t pin, int val) {
    pinValues[pin] = val;
}

unsigned long millis(void) {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return duration.count();
}

unsigned long micros(void) {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime);
    return duration.count();
}

void delay(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void delayMicroseconds(unsigned int us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

// Mock Serial implementation
void HardwareSerial::begin(unsigned long baud) {
    // Mock - do nothing
}

void HardwareSerial::end() {
    // Mock - do nothing
}

int HardwareSerial::available(void) {
    return 0; // Mock - no data available
}

int HardwareSerial::read(void) {
    return -1; // Mock - no data
}

int HardwareSerial::peek(void) {
    return -1; // Mock - no data
}

void HardwareSerial::flush(void) {
    // Mock - do nothing
}

size_t HardwareSerial::write(uint8_t c) {
    return 1; // Mock - always success
}

size_t HardwareSerial::write(const char *str) {
    return strlen(str); // Mock - always success
}

size_t HardwareSerial::println(const char *str) {
    return write(str);
}

size_t HardwareSerial::println(int value) {
    return 1;
}

size_t HardwareSerial::println(unsigned long value) {
    return 1;
}

size_t HardwareSerial::print(const char *str) {
    return write(str);
}

size_t HardwareSerial::print(int value) {
    return 1;
}

size_t HardwareSerial::print(unsigned long value) {
    return 1;
}

HardwareSerial Serial;
