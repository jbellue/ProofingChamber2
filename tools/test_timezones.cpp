// Simple test to verify Timezones.h structure
#include "../src/Timezones.h"
#include <iostream>
#include <cstring>

int main() {
    std::cout << "Testing generated Timezones.h..." << std::endl;
    
    // Test that we have timezones
    if (timezones::CONTINENT_COUNT == 0) {
        std::cerr << "ERROR: No continents found!" << std::endl;
        return 1;
    }
    
    std::cout << "Found " << timezones::CONTINENT_COUNT << " continents" << std::endl;
    
    // Test that Europe exists and has Paris
    bool foundEurope = false;
    bool foundParis = false;
    
    for (int c = 0; c < timezones::CONTINENT_COUNT; c++) {
        const timezones::Continent& continent = timezones::CONTINENTS[c];
        std::cout << "  - " << continent.name << ": " << continent.count << " timezones" << std::endl;
        
        if (strcmp(continent.name, "Europe") == 0) {
            foundEurope = true;
            
            // Look for Paris
            for (int i = 0; i < continent.count; i++) {
                if (strcmp(continent.timezones[i].name, "Paris") == 0) {
                    foundParis = true;
                    std::cout << "    Found Paris: " << continent.timezones[i].posixString << std::endl;
                    break;
                }
            }
        }
    }
    
    if (!foundEurope) {
        std::cerr << "ERROR: Europe continent not found!" << std::endl;
        return 1;
    }
    
    if (!foundParis) {
        std::cerr << "ERROR: Paris timezone not found in Europe!" << std::endl;
        return 1;
    }
    
    // Test defaults
    std::cout << "Default continent: " << timezones::CONTINENTS[timezones::DEFAULT_CONTINENT_INDEX].name << std::endl;
    std::cout << "Default timezone: " << timezones::CONTINENTS[timezones::DEFAULT_CONTINENT_INDEX].timezones[timezones::DEFAULT_TIMEZONE_INDEX].name << std::endl;
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
