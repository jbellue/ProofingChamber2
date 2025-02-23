#include "Storage.h"

Storage::Storage() {
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return;
    }
    Serial.println("LittleFS mounted successfully");
}

int Storage::readIntFromFile(const char* path, int defaultValue) {
    Serial.printf("Reading file: %s\n", path);

    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return defaultValue;
    }

    String content = file.readString();
    file.close();
    return content.toInt();
}

float Storage::readFloatFromFile(const char* path, float defaultValue) {
    Serial.printf("Reading file: %s\n", path);

    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return defaultValue;
    }

    String content = file.readString();
    file.close();
    return content.toFloat();
}

String Storage::readStringFromFile(const char* path, const String& defaultValue) {
    Serial.printf("Reading file: %s\n", path);

    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
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
    Serial.printf("Writing to file: %s\n", path);

    File file = LittleFS.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }

    if (file.print(value)) {
        Serial.println("File written successfully");
        file.close();
        return true;
    } else {
        Serial.println("Write failed");
        file.close();
        return false;
    }
}
