#pragma once
#include "IStorage.h"

namespace services {
    struct StorageAdapter : public IStorage {
        bool begin() override;
        int getInt(const char* path, int defaultValue = 0) override;
        float getFloat(const char* path, float defaultValue = 0.0f) override;
        bool getCharArray(const char* path, char* buffer, size_t bufferSize, const char* defaultValue = "") override;
        bool setInt(const char* path, int value) override;
        bool setFloat(const char* path, float value) override;
        bool setCharArray(const char* path, const char* value) override;
    };
}
