#pragma once

#include "services/IStorage.h"

namespace services {
// Null Object Pattern: Safe do-nothing implementation
class NullStorage : public IStorage {
public:
    static NullStorage& getInstance() {
        static NullStorage instance;
        return instance;
    }

    bool begin() override {
        return true;
    }

    int readInt(const char* path, int defaultValue = 0) override {
        (void)path;
        return defaultValue;
    }

    float readFloat(const char* path, float defaultValue = 0.0f) override {
        (void)path;
        return defaultValue;
    }

    bool readString(const char* path, char* buffer, size_t bufferSize, const char* defaultValue = "") override {
        (void)path;
        return false;
    }

    bool writeInt(const char* path, int value) override {
        (void)path;
        (void)value;
        return false;
    }

    bool writeFloat(const char* path, float value) override {
        (void)path;
        (void)value;
        return false;
    }

    bool writeString(const char* path, const char* value) override {
        (void)path;
        (void)value;
        return false;
    }

private:
    NullStorage() = default;
    NullStorage(const NullStorage&) = delete;
    NullStorage& operator=(const NullStorage&) = delete;
};
} // namespace services
