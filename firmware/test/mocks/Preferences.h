// Mock Preferences.h for unit testing
#pragma once

#include <stdint.h>
#include <string>
#include <map>
#include <cstring>

class Preferences {
private:
    std::string namespace_name;
    bool is_readonly;
    std::map<std::string, int32_t> int_storage;
    std::map<std::string, float> float_storage;
    std::map<std::string, std::string> string_storage;
    
public:
    Preferences() : is_readonly(false) {}
    
    bool begin(const char *name, bool readOnly = false) {
        namespace_name = name;
        is_readonly = readOnly;
        return true;
    }
    
    void end() {
        namespace_name = "";
    }
    
    bool isKey(const char *key) {
        std::string k(key);
        return int_storage.count(k) || float_storage.count(k) || string_storage.count(k);
    }
    
    bool putInt(const char *key, int32_t value) {
        if (is_readonly) return false;
        int_storage[key] = value;
        return true;
    }
    
    int32_t getInt(const char *key, int32_t defaultValue = 0) {
        if (int_storage.count(key)) {
            return int_storage[key];
        }
        return defaultValue;
    }
    
    bool putFloat(const char *key, float value) {
        if (is_readonly) return false;
        float_storage[key] = value;
        return true;
    }
    
    float getFloat(const char *key, float defaultValue = 0.0f) {
        if (float_storage.count(key)) {
            return float_storage[key];
        }
        return defaultValue;
    }
    
    bool putString(const char *key, const char *value) {
        if (is_readonly) return false;
        string_storage[key] = value;
        return true;
    }
    
    size_t getString(const char *key, char *value, size_t maxLen) {
        if (string_storage.count(key)) {
            std::string str = string_storage[key];
            size_t len = str.length();
            if (len >= maxLen) len = maxLen - 1;
            strncpy(value, str.c_str(), len);
            value[len] = '\0';
            return len;
        }
        value[0] = '\0';
        return 0;
    }
    
    // Helper for tests
    void clear() {
        int_storage.clear();
        float_storage.clear();
        string_storage.clear();
    }
};
