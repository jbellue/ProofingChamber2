// Mock Arduino.h for unit testing
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

// Basic Arduino types
typedef bool boolean;
typedef uint8_t byte;

// Arduino constants
#define HIGH 0x1
#define LOW  0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

// Mock Arduino functions
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int val);
unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);

// Mock Serial class
class HardwareSerial {
public:
    void begin(unsigned long baud);
    void end();
    int available(void);
    int read(void);
    int peek(void);
    void flush(void);
    size_t write(uint8_t);
    size_t write(const char *str);
    size_t println(const char *);
    size_t println(int);
    size_t println(unsigned long);
    size_t print(const char *);
    size_t print(int);
    size_t print(unsigned long);
};

extern HardwareSerial Serial;

// Mock String class
class String {
private:
    std::string data;
    
public:
    String() : data("") {}
    String(const char *cstr) : data(cstr ? cstr : "") {}
    String(const std::string &str) : data(str) {}
    String(const String &str) : data(str.data) {}
    String(int value) : data(std::to_string(value)) {}
    String(unsigned long value) : data(std::to_string(value)) {}
    
    ~String() {}
    
    unsigned int length() const { return data.length(); }
    const char * c_str() const { return data.c_str(); }
    operator const char*() const { return data.c_str(); }
    
    String& operator=(const String &rhs) {
        if (this != &rhs) {
            data = rhs.data;
        }
        return *this;
    }
    
    String& operator=(const char *cstr) {
        data = cstr ? cstr : "";
        return *this;
    }
    
    bool operator==(const String &rhs) const {
        return data == rhs.data;
    }
    
    bool operator!=(const String &rhs) const {
        return data != rhs.data;
    }
};
