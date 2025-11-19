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

String StorageAdapter::readString(const char* path, const String& defaultValue) {
    return Storage::readStringFromFile(path, defaultValue);
}

bool StorageAdapter::writeInt(const char* path, int value) {
    return Storage::writeIntToFile(path, value);
}

bool StorageAdapter::writeFloat(const char* path, float value) {
    return Storage::writeFloatToFile(path, value);
}

bool StorageAdapter::writeString(const char* path, const String& value) {
    return Storage::writeStringToFile(path, value);
}

} // namespace services
