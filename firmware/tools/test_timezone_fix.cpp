// Test to verify the timezone index fix
#include "../src/Timezones.h"
#include "../src/TimezoneHelpers.h"
#include <iostream>
#include <cstring>

int main() {
    std::cout << "Testing timezone index fix..." << std::endl;
    
    // Test that multiple timezones can share the same POSIX string
    // but can be distinguished by their index
    
    std::cout << "\nChecking for duplicate POSIX strings:" << std::endl;
    for (int i = 0; i < timezones::TIMEZONE_COUNT; i++) {
        for (int j = i + 1; j < timezones::TIMEZONE_COUNT; j++) {
            if (strcmp(timezones::TIMEZONES[i].posixString, timezones::TIMEZONES[j].posixString) == 0) {
                std::cout << "  Found duplicate: " 
                          << timezones::TIMEZONES[i].continent << "/" << timezones::TIMEZONES[i].name
                          << " (index " << i << ") and "
                          << timezones::TIMEZONES[j].continent << "/" << timezones::TIMEZONES[j].name
                          << " (index " << j << ")"
                          << " share: " << timezones::TIMEZONES[i].posixString
                          << std::endl;
            }
        }
    }
    
    // Test getTimezoneGlobalIndex
    std::cout << "\nTesting getTimezoneGlobalIndex:" << std::endl;
    
    // Test each timezone
    for (int i = 0; i < timezones::TIMEZONE_COUNT; i++) {
        const char* continent = timezones::TIMEZONES[i].continent;
        const char* name = timezones::TIMEZONES[i].name;
        
        // Find the local index for this timezone
        int localIndex = 0;
        for (int j = 0; j < i; j++) {
            if (strcmp(timezones::TIMEZONES[j].continent, continent) == 0) {
                localIndex++;
            }
        }
        
        // Get the global index using the helper function
        int globalIndex = timezones::getTimezoneGlobalIndex(continent, localIndex);
        
        if (globalIndex != i) {
            std::cerr << "ERROR: getTimezoneGlobalIndex(" << continent << ", " << localIndex 
                      << ") returned " << globalIndex << " but expected " << i << std::endl;
            return 1;
        }
        
        std::cout << "  ✓ " << continent << "/" << name << " (global: " << i 
                  << ", local: " << localIndex << ")" << std::endl;
    }
    
    // Test that findTimezoneIndex still works for backward compatibility
    std::cout << "\nTesting findTimezoneIndex (backward compatibility):" << std::endl;
    const char* parisPostix = "CET-1CEST,M3.5.0,M10.5.0/3";
    int parisIndex = timezones::findTimezoneIndex(parisPostix);
    std::cout << "  findTimezoneIndex(\"" << parisPostix << "\") = " << parisIndex << std::endl;
    std::cout << "  Result: " << timezones::TIMEZONES[parisIndex].continent 
              << "/" << timezones::TIMEZONES[parisIndex].name << std::endl;
    std::cout << "  (Note: Returns first match, which is expected for backward compatibility)" << std::endl;
    
    std::cout << "\n✅ All tests passed!" << std::endl;
    return 0;
}
