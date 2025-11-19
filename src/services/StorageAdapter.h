#pragma once
#include "IStorage.h"

namespace services {
    struct StorageAdapter : public IStorage {
        bool begin() override;
        int readInt(const char* path, int defaultValue = 0) override;
        float readFloat(const char* path, float defaultValue = 0.0f) override;
        String readString(const char* path, const String& defaultValue = String()) override;
        bool writeInt(const char* path, int value) override;
        bool writeFloat(const char* path, float value) override;
        bool writeString(const char* path, const String& value) override;
    };
}
