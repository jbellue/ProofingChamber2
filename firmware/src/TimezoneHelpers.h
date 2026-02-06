#pragma once

#include "Timezones.h"
#include <cstring>  // for strcmp

// ============================================================================
// Timezone Helper Functions
// ============================================================================
//
// This file contains helper functions for working with the timezone data
// defined in Timezones.h. These functions provide convenient access patterns
// for continent-based navigation and timezone lookups.
//
// ============================================================================

namespace timezones {

    // Get count of unique continents
    inline int getContinentCount() {
        int count = 0;
        const char* lastContinent = nullptr;
        for (int i = 0; i < TIMEZONE_COUNT; i++) {
            if (!lastContinent || strcmp(TIMEZONES[i].continent, lastContinent) != 0) {
                count++;
                lastContinent = TIMEZONES[i].continent;
            }
        }
        return count;
    }

    // Get continent name by index (0-based)
    inline const char* getContinentName(int continentIndex) {
        int currentContinent = -1;
        const char* lastContinent = nullptr;
        for (int i = 0; i < TIMEZONE_COUNT; i++) {
            if (!lastContinent || strcmp(TIMEZONES[i].continent, lastContinent) != 0) {
                currentContinent++;
                lastContinent = TIMEZONES[i].continent;
                if (currentContinent == continentIndex) {
                    return lastContinent;
                }
            }
        }
        return nullptr;
    }

    // Get timezone count for a continent
    inline int getTimezoneCount(const char* continent) {
        int count = 0;
        for (int i = 0; i < TIMEZONE_COUNT; i++) {
            if (strcmp(TIMEZONES[i].continent, continent) == 0) {
                count++;
            }
        }
        return count;
    }

    // Get timezone by continent and local index
    inline const Timezone* getTimezone(const char* continent, int localIndex) {
        int currentIndex = -1;
        for (int i = 0; i < TIMEZONE_COUNT; i++) {
            if (strcmp(TIMEZONES[i].continent, continent) == 0) {
                currentIndex++;
                if (currentIndex == localIndex) {
                    return &TIMEZONES[i];
                }
            }
        }
        return nullptr;
    }

    // Get global timezone index by continent and local index
    inline int getTimezoneGlobalIndex(const char* continent, int localIndex) {
        int currentIndex = -1;
        for (int i = 0; i < TIMEZONE_COUNT; i++) {
            if (strcmp(TIMEZONES[i].continent, continent) == 0) {
                currentIndex++;
                if (currentIndex == localIndex) {
                    return i;
                }
            }
        }
        return DEFAULT_TIMEZONE_INDEX;
    }

    // Find timezone global index by POSIX string
    inline int findTimezoneIndex(const char* posixString) {
        for (int i = 0; i < TIMEZONE_COUNT; i++) {
            if (strcmp(TIMEZONES[i].posixString, posixString) == 0) {
                return i;
            }
        }
        return DEFAULT_TIMEZONE_INDEX;
    }

}  // namespace timezones
