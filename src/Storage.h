#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <LittleFS.h>

class Storage {
public:
    // Constructor
    Storage() = delete; // Prevent instantiation of this class

    // Method to initialize the file system
    static bool begin();

    // Method to read int from a file
    static int readIntFromFile(const char* path, int defaultValue = 0);

    // Method to read float from a file
    static float readFloatFromFile(const char* path, float defaultValue = 0.0);

    // Method to read String from a file
    static String readStringFromFile(const char* path, const String& defaultValue = "");

    // Method to write int to a file
    static bool writeIntToFile(const char* path, int value);

    // Method to write float to a file
    static bool writeFloatToFile(const char* path, float value);

    // Method to write String to a file
    static bool writeStringToFile(const char* path, const String& value);

private:
    // Helper method to write data to a file
    static bool writeToFile(const char* path, const String& value);
    static bool _initialized;  // Track initialization state
};

#endif