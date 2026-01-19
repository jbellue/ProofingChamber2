// Simple test to verify Timezones.h structure
#include "../src/Timezones.h"
#include "../src/TimezoneHelpers.h"
#include <iostream>
#include <cstring>

int main() {
    std::cout << "Testing generated Timezones.h..." << std::endl;
    
    // Test that we have timezones
    if (timezones::TIMEZONE_COUNT == 0) {
        std::cerr << "ERROR: No timezones found!" << std::endl;
        return 1;
    }
    
    std::cout << "Found " << timezones::TIMEZONE_COUNT << " timezones" << std::endl;
    
    // Test continent count
    int continentCount = timezones::getContinentCount();
    std::cout << "Found " << continentCount << " continents" << std::endl;
    
    // List all continents
    for (int c = 0; c < continentCount; c++) {
        const char* continentName = timezones::getContinentName(c);
        int tzCount = timezones::getTimezoneCount(continentName);
        std::cout << "  - " << continentName << ": " << tzCount << " timezones" << std::endl;
    }
    
    // Test that Europe/Paris exists
    bool foundParis = false;
    int europeCount = timezones::getTimezoneCount("Europe");
    
    if (europeCount == 0) {
        std::cerr << "ERROR: Europe continent not found!" << std::endl;
        return 1;
    }
    
    // Look for Paris
    for (int i = 0; i < europeCount; i++) {
        const timezones::Timezone* tz = timezones::getTimezone("Europe", i);
        if (tz && strcmp(tz->name, "Paris") == 0) {
            foundParis = true;
            std::cout << "    Found Paris: " << tz->posixString << std::endl;
            break;
        }
    }
    
    if (!foundParis) {
        std::cerr << "ERROR: Paris timezone not found in Europe!" << std::endl;
        return 1;
    }
    
    // Test default timezone
    const timezones::Timezone& defaultTz = timezones::TIMEZONES[timezones::DEFAULT_TIMEZONE_INDEX];
    std::cout << "Default timezone: " << defaultTz.continent << "/" << defaultTz.name << std::endl;
    
    if (strcmp(defaultTz.continent, "Europe") != 0 || strcmp(defaultTz.name, "Paris") != 0) {
        std::cerr << "ERROR: Default timezone is not Europe/Paris!" << std::endl;
        return 1;
    }
    
    // Test findTimezoneIndex
    int parisIndex = timezones::findTimezoneIndex("CET-1CEST,M3.5.0,M10.5.0/3");
    if (parisIndex < 0 || parisIndex >= timezones::TIMEZONE_COUNT) {
        std::cerr << "ERROR: findTimezoneIndex failed!" << std::endl;
        return 1;
    }
    std::cout << "findTimezoneIndex test passed (Paris at index " << parisIndex << ")" << std::endl;
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
