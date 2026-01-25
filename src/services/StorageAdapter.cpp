#include "StorageAdapter.h"
#include "../Storage.h"

namespace services {

bool StorageAdapter::begin() {
    return Storage::begin();
}

int StorageAdapter::getInt(const char* path, const int defaultValue) {
    return Storage::getInt(path, defaultValue);
}

float StorageAdapter::getFloat(const char* path, const float defaultValue) {
    return Storage::getFloat(path, defaultValue);
}

bool StorageAdapter::getCharArray(const char* path, char* buffer, const size_t bufferSize, const char* defaultValue) {
    return Storage::getCharArray(path, buffer, bufferSize, defaultValue);
}

bool StorageAdapter::setInt(const char* path, const int value) {
    return Storage::setInt(path, value);
}

bool StorageAdapter::setFloat(const char* path, const float value) {
    return Storage::setFloat(path, value);
}

bool StorageAdapter::setCharArray(const char* path, const char* value) {
    return Storage::setCharArray(path, value);
}

} // namespace services
