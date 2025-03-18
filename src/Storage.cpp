#include "DebugUtils.h"
#include "Storage.h"

Storage::Storage() {
    if (!LittleFS.begin()) {
        DEBUG_PRINTLN("Failed to mount LittleFS");
        return;
    }
    DEBUG_PRINTLN("LittleFS mounted successfully");
}

int Storage::readIntFromFile(const char* path, int defaultValue) {
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
    return writeToFile(path, String(value));
}

bool Storage::writeFloatToFile(const char* path, float value) {
    return writeToFile(path, String(value));
}

bool Storage::writeStringToFile(const char* path, const String& value) {
    return writeToFile(path, value);
}

bool Storage::writeToFile(const char* path, const String& value) {
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
