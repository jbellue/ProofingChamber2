#include "DebugUtils.h"
#include "Storage.h"

bool Storage::_initialized = false;

bool Storage::begin() {
    if (_initialized) {
        return true;
    }

    if (!LittleFS.begin()) {
        DEBUG_PRINTLN("Failed to mount LittleFS");
        return false;
    }

    DEBUG_PRINTLN("LittleFS mounted successfully");
    _initialized = true;
    return true;
}

int Storage::readIntFromFile(const char* path, int defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return defaultValue;
    }
    DEBUG_PRINT("Reading int from file ");
    DEBUG_PRINTLN(path);

    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        DEBUG_PRINTLN("Failed to open file for reading");
        return defaultValue;
    }

    String content = file.readString();
    file.close();
    return content.toInt();
}

float Storage::readFloatFromFile(const char* path, float defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return defaultValue;
    }
    DEBUG_PRINT("Reading float from file ");
    DEBUG_PRINTLN(path);

    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        DEBUG_PRINTLN("Failed to open file for reading");
        return defaultValue;
    }

    char buffer[32];
    size_t len = file.readBytes(buffer, sizeof(buffer) - 1);
    file.close();
    
    if (len == 0) {
        return defaultValue;
    }
    
    buffer[len] = '\0';
    return atof(buffer);
}

bool Storage::readStringFromFile(const char* path, char* buffer, size_t bufferSize, const char* defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        strncpy(buffer, defaultValue, bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
        return false;
    }
    DEBUG_PRINT("Reading string from file ");
    DEBUG_PRINTLN(path);

    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        DEBUG_PRINTLN("Failed to open file for reading");
        strncpy(buffer, defaultValue, bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
        return false;
    }

    size_t len = file.readBytes(buffer, bufferSize - 1);
    file.close();
    
    if (len == 0) {
        strncpy(buffer, defaultValue, bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
        return false;
    }
    
    buffer[len] = '\0';
    return true;
}

bool Storage::writeIntToFile(const char* path, int value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return writeToFile(path, buffer);
}

bool Storage::writeFloatToFile(const char* path, float value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    return writeToFile(path, buffer);
}

bool Storage::writeStringToFile(const char* path, const char* value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    return writeToFile(path, value);
}

bool Storage::writeToFile(const char* path, const char* value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    DEBUG_PRINT("Writing to file ");
    DEBUG_PRINTLN(path);

    File file = LittleFS.open(path, FILE_WRITE);
    if (!file) {
        DEBUG_PRINTLN("Failed to open file for writing");
        return false;
    }

    size_t written = file.print(value);
    file.close();
    
    if (written > 0) {
        DEBUG_PRINTLN("File written successfully");
        return true;
    } else {
        DEBUG_PRINTLN("Write failed");
        return false;
    }
}
