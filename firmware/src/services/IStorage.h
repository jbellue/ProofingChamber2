#pragma once
#include <Arduino.h>

namespace services {
    struct IStorage {
        virtual ~IStorage() {}
        virtual bool begin() = 0;
        virtual int getInt(const char* path, const int defaultValue = 0) = 0;
        virtual float getFloat(const char* path, const float defaultValue = 0.0f) = 0;
        virtual bool getCharArray(const char* path, char* buffer, const size_t bufferSize, const char* defaultValue = "") = 0;
        virtual bool setInt(const char* path, const int value) = 0;
        virtual bool setFloat(const char* path, const float value) = 0;
        virtual bool setCharArray(const char* path, const char* value) = 0;
    };
}
