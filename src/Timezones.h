#pragma once

namespace timezones {
    struct Timezone {
        const char* name;
        const char* posixString;
    };

    struct Continent {
        const char* name;
        const Timezone* timezones;
        int count;
    };

    // Europe timezones
    static const Timezone EUROPE[] = {
        {"Paris", "CET-1CEST,M3.5.0,M10.5.0"},
        {"London", "GMT0BST,M3.5.0/1,M10.5.0"},
        {"Berlin", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Madrid", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Rome", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Amsterdam", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Brussels", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Vienna", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Prague", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Warsaw", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Budapest", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Athens", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
        {"Helsinki", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
        {"Istanbul", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
        {"Moscow", "MSK-3"},
    };

    // North America timezones
    static const Timezone NORTH_AMERICA[] = {
        {"New York", "EST5EDT,M3.2.0,M11.1.0"},
        {"Chicago", "CST6CDT,M3.2.0,M11.1.0"},
        {"Denver", "MST7MDT,M3.2.0,M11.1.0"},
        {"Los Angeles", "PST8PDT,M3.2.0,M11.1.0"},
    };

    // Asia timezones
    static const Timezone ASIA[] = {
        {"Tokyo", "JST-9"},
        {"Shanghai", "CST-8"},
        {"Hong Kong", "HKT-8"},
        {"Singapore", "SGT-8"},
        {"Bangkok", "ICT-7"},
        {"Dubai", "GST-4"},
    };

    // Australia timezones
    static const Timezone AUSTRALIA[] = {
        {"Sydney", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
        {"Melbourne", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
        {"Brisbane", "AEST-10"},
        {"Perth", "AWST-8"},
    };

    // All continents
    static const Continent CONTINENTS[] = {
        {"Europe", EUROPE, sizeof(EUROPE) / sizeof(Timezone)},
        {"North America", NORTH_AMERICA, sizeof(NORTH_AMERICA) / sizeof(Timezone)},
        {"Asia", ASIA, sizeof(ASIA) / sizeof(Timezone)},
        {"Australia", AUSTRALIA, sizeof(AUSTRALIA) / sizeof(Timezone)},
    };

    static const int CONTINENT_COUNT = sizeof(CONTINENTS) / sizeof(Continent);
    static const int DEFAULT_CONTINENT_INDEX = 0;  // Europe
    static const int DEFAULT_TIMEZONE_INDEX = 0;   // Paris
}


