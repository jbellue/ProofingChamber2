#include "DebugUtils.h"
#include "Storage.h"

bool Storage::_initialized = false;
static Preferences preferences;

// Helper function to convert file path to preference key
// Converts "/timezone.txt" -> "timezone"
// Converts "/hot/lower_limit.txt" -> "hot_lower_limit"
// Converts "/cold/higher_limit.txt" -> "cold_higher_limit"
static String pathToKey(const char* path) {
    String key = String(path);
    // Remove leading slash
    if (key.startsWith("/")) {
        key = key.substring(1);
    }
    // Remove file extension
    int dotIndex = key.lastIndexOf('.');
    if (dotIndex > 0) {
        key = key.substring(0, dotIndex);
    }
    // Replace slashes with underscores
    key.replace('/', '_');
    return key;
}

bool Storage::begin() {
    if (_initialized) {
        return true;
    }

    // Open Preferences with namespace "storage" in read-write mode
    if (!preferences.begin("storage", false)) {
        DEBUG_PRINTLN("Failed to initialize Preferences");
        return false;
    }

    DEBUG_PRINTLN("Preferences initialized successfully");
    _initialized = true;
    return true;
}

int Storage::readIntFromFile(const char* path, int defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return defaultValue;
    }
    DEBUG_PRINT("Reading int from key ");
    DEBUG_PRINTLN(path);

    String key = pathToKey(path);
    return preferences.getInt(key.c_str(), defaultValue);
}

float Storage::readFloatFromFile(const char* path, float defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return defaultValue;
    }
    DEBUG_PRINT("Reading float from key ");
    DEBUG_PRINTLN(path);

    String key = pathToKey(path);
    return preferences.getFloat(key.c_str(), defaultValue);
}

bool Storage::readStringFromFile(const char* path, char* buffer, size_t bufferSize, const char* defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        strncpy(buffer, defaultValue, bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
        return false;
    }
    DEBUG_PRINT("Reading string from key ");
    DEBUG_PRINTLN(path);

    String key = pathToKey(path);
    String value = preferences.getString(key.c_str(), defaultValue);
    
    strncpy(buffer, value.c_str(), bufferSize - 1);
    buffer[bufferSize - 1] = '\0';
    
    return value.length() > 0;
}

bool Storage::writeIntToFile(const char* path, int value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    DEBUG_PRINT("Writing int to key ");
    DEBUG_PRINTLN(path);

    String key = pathToKey(path);
    size_t written = preferences.putInt(key.c_str(), value);
    
    if (written > 0) {
        DEBUG_PRINTLN("Value written successfully");
        return true;
    } else {
        DEBUG_PRINTLN("Write failed");
        return false;
    }
}

bool Storage::writeFloatToFile(const char* path, float value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    DEBUG_PRINT("Writing float to key ");
    DEBUG_PRINTLN(path);

    String key = pathToKey(path);
    size_t written = preferences.putFloat(key.c_str(), value);
    
    if (written > 0) {
        DEBUG_PRINTLN("Value written successfully");
        return true;
    } else {
        DEBUG_PRINTLN("Write failed");
        return false;
    }
}

bool Storage::writeStringToFile(const char* path, const char* value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    DEBUG_PRINT("Writing string to key ");
    DEBUG_PRINTLN(path);

    String key = pathToKey(path);
    size_t written = preferences.putString(key.c_str(), value);
    
    if (written > 0) {
        DEBUG_PRINTLN("Value written successfully");
        return true;
    } else {
        DEBUG_PRINTLN("Write failed");
        return false;
    }
}
