#pragma once
#include <Arduino.h>

namespace services {
    struct IStorage {
        virtual ~IStorage() {}
        virtual bool begin() = 0;
        virtual int readInt(const char* path, int defaultValue = 0) = 0;
        virtual float readFloat(const char* path, float defaultValue = 0.0f) = 0;
        virtual String readString(const char* path, const String& defaultValue = String()) = 0;
        virtual bool writeInt(const char* path, int value) = 0;
        virtual bool writeFloat(const char* path, float value) = 0;
        virtual bool writeString(const char* path, const String& value) = 0;
    };
}
