#include "StorageAdapter.h"
#include "../Storage.h"

namespace services {

bool StorageAdapter::begin() {
    return Storage::begin();
}

int StorageAdapter::readInt(const char* path, int defaultValue) {
    return Storage::readIntFromFile(path, defaultValue);
}

float StorageAdapter::readFloat(const char* path, float defaultValue) {
    return Storage::readFloatFromFile(path, defaultValue);
}

bool StorageAdapter::readString(const char* path, char* buffer, size_t bufferSize, const char* defaultValue) {
    return Storage::readStringFromFile(path, buffer, bufferSize, defaultValue);
}

bool StorageAdapter::writeInt(const char* path, int value) {
    return Storage::writeIntToFile(path, value);
}

bool StorageAdapter::writeFloat(const char* path, float value) {
    return Storage::writeFloatToFile(path, value);
}

bool StorageAdapter::writeString(const char* path, const char* value) {
    return Storage::writeStringToFile(path, value);
}

} // namespace services
