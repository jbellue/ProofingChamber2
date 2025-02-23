#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <LittleFS.h>

class Storage {
public:
    // Constructor
    Storage();

    // Method to read int from a file
    int readIntFromFile(const char* path, int defaultValue);

    // Method to read float from a file
    float readFloatFromFile(const char* path, float defaultValue);

    // Method to read String from a file
    String readStringFromFile(const char* path, const String& defaultValue);

    // Method to write int to a file
    bool writeIntToFile(const char* path, int value);

    // Method to write float to a file
    bool writeFloatToFile(const char* path, float value);

    // Method to write String to a file
    bool writeStringToFile(const char* path, const String& value);

private:
    // Helper method to write data to a file
    bool writeToFile(const char* path, const String& value);
};

#endif