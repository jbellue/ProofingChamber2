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

    String content = file.readString();
    file.close();
    return content.toFloat();
}

String Storage::readStringFromFile(const char* path, const String& defaultValue) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return defaultValue;
    }
    DEBUG_PRINT("Reading string from file ");
    DEBUG_PRINTLN(path);

    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        DEBUG_PRINTLN("Failed to open file for reading");
        return defaultValue;
    }

    String content = file.readString();
    file.close();
    return content;
}

bool Storage::writeIntToFile(const char* path, int value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    return writeToFile(path, String(value));
}

bool Storage::writeFloatToFile(const char* path, float value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    return writeToFile(path, String(value));
}

bool Storage::writeStringToFile(const char* path, const String& value) {
    if (!_initialized) {
        DEBUG_PRINTLN("Storage not initialized");
        return false;
    }
    return writeToFile(path, value);
}

bool Storage::writeToFile(const char* path, const String& value) {
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

    if (file.print(value)) {
        DEBUG_PRINTLN("File written successfully");
        file.close();
        return true;
    } else {
        DEBUG_PRINTLN("Write failed");
        file.close();
        return false;
    }
}
