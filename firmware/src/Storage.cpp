#include "DebugUtils.h"
#include "Storage.h"
#include "StorageConstants.h"

bool Storage::_initialized = false;
static Preferences preferences;

bool Storage::begin() {
    if (_initialized) {
        return true;
    }

    // Open Preferences with namespace "storage" in read-write mode
    if (!preferences.begin("storage", false)) {
        DEBUG_PRINTLN("Failed to initialize Preferences");
        return false;
    }

    InitKeyIfMissing(storage::keys::HOT_LOWER_LIMIT_KEY, storage::defaults::HOT_LOWER_LIMIT_DEFAULT);
    InitKeyIfMissing(storage::keys::HOT_UPPER_LIMIT_KEY, storage::defaults::HOT_UPPER_LIMIT_DEFAULT);
    InitKeyIfMissing(storage::keys::COLD_LOWER_LIMIT_KEY, storage::defaults::COLD_LOWER_LIMIT_DEFAULT);
    InitKeyIfMissing(storage::keys::COLD_UPPER_LIMIT_KEY, storage::defaults::COLD_UPPER_LIMIT_DEFAULT);
    InitKeyIfMissing(storage::keys::TIMEZONE_KEY, storage::defaults::TIMEZONE_DEFAULT);

    DEBUG_PRINTLN("Preferences initialized successfully");
    _initialized = true;
    return true;
}

int Storage::getInt(const char* path, const int defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return defaultValue;
    }
    DEBUG_PRINT("Reading int from key ");
    DEBUG_PRINTLN(path);

    return preferences.getInt(path, defaultValue);
}

float Storage::getFloat(const char* path, const float defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return defaultValue;
    }
    DEBUG_PRINT("Reading float from key ");
    DEBUG_PRINTLN(path);

    return preferences.getFloat(path, defaultValue);
}

bool Storage::getCharArray(const char* path, char* buffer, const size_t bufferSize, const char* defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        strncpy(buffer, defaultValue, bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
        return false;
    }
    DEBUG_PRINT("Reading string from key ");
    DEBUG_PRINTLN(path);

    // Check if the key exists in preferences
    bool keyExists = preferences.isKey(path);
    
    if (keyExists) {
        preferences.getString(path, buffer, bufferSize);
        DEBUG_PRINT("Read string: ");
        DEBUG_PRINTLN(buffer);
    } else {
        DEBUG_PRINTLN("Key not found, using default value");
        strncpy(buffer, defaultValue, bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
    }
    
    return keyExists;
}

bool Storage::setInt(const char* path, const int value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    DEBUG_PRINT("Writing int to key ");
    DEBUG_PRINTLN(path);

    const size_t written = preferences.putInt(path, value);
    
    if (written > 0) {
        DEBUG_PRINTLN("Value written successfully");
        return true;
    } else {
        DEBUG_PRINTLN("Write failed");
        return false;
    }
}

bool Storage::setFloat(const char* path, const float value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    DEBUG_PRINT("Writing float to key ");
    DEBUG_PRINTLN(path);

    const size_t written = preferences.putFloat(path, value);
    
    if (written > 0) {
        DEBUG_PRINTLN("Value written successfully");
        return true;
    } else {
        DEBUG_PRINTLN("Write failed");
        return false;
    }
}

bool Storage::setCharArray(const char* path, const char* value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    DEBUG_PRINT("Writing string to key ");
    DEBUG_PRINTLN(path);

    const size_t written = preferences.putString(path, value);
    
    if (written > 0) {
        DEBUG_PRINTLN("Value written successfully");
        return true;
    } else {
        DEBUG_PRINTLN("Write failed");
        return false;
    }
}

void Storage::InitKeyIfMissing(const char* key, const int defaultValue) {
    if (!preferences.isKey(key)) {
        preferences.putInt(key, defaultValue);
    }
}
void Storage::InitKeyIfMissing(const char* key, const char* defaultValue) {
    if (!preferences.isKey(key)) {
        preferences.putString(key, defaultValue);
    }
}