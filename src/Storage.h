#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <Preferences.h>

class Storage {
public:
    // Constructor
    Storage() = delete; // Prevent instantiation of this class

    // Method to initialize the file system
    static bool begin();

    // Method to read int from a file
    static int getInt(const char* path, const int defaultValue = 0);

    // Method to read float from a file
    static float getFloat(const char* path, const float defaultValue = 0.0);

    // Method to read String from a file
    static bool getCharArray(const char* path, char* buffer, const size_t bufferSize, const char* defaultValue = "");

    // Method to write int to a file
    static bool setInt(const char* path, const int value);

    // Method to write float to a file
    static bool setFloat(const char* path, const float value);
    // Method to write String to a file
    static bool setCharArray(const char* path, const char* value);

private:
    static bool _initialized;  // Track initialization state

    static void InitKeyIfMissing(const char* key, const int defaultValue);
    static void InitKeyIfMissing(const char* key, const char* defaultValue);

};

#endif