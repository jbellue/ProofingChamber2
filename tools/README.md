# Timezone Generation

This directory contains the timezone database and generation scripts for the Proofing Chamber.

## Overview

The timezone list is now automatically generated from the [posix_tz_db](https://github.com/nayarsystems/posix_tz_db) repository, which provides a comprehensive database of POSIX timezone strings.

## Files

- `posix_tz_db/` - Git submodule containing the timezone database (zones.csv, zones.json)
- `generate_timezones.py` - Python script that parses zones.csv and generates `src/Timezones.h`
- `pre_build.py` - PlatformIO extra script that runs the generator before each build

## How it Works

1. **Build-time Generation**: When you build the project with PlatformIO, the `pre_build.py` script automatically runs `generate_timezones.py`
2. **CSV Parsing**: The generator reads `posix_tz_db/zones.csv` which contains 461 timezones
3. **Header Generation**: Creates `src/Timezones.h` with:
   - Timezone data organized by continent
   - 11 continents (Africa, America, Antarctica, Arctic, Asia, Atlantic, Australia, Etc, Europe, Indian, Pacific)
   - Same API structure as the previous static list
   - Default timezone set to Europe/Paris

## Manual Generation

To manually regenerate the timezone header:

```bash
python3 tools/generate_timezones.py
```

## Updating the Timezone Database

To update to the latest timezone data:

```bash
cd tools/posix_tz_db
git pull origin master
cd ../..
git add tools/posix_tz_db
git commit -m "Update timezone database"
```

## Generated File

The generated `src/Timezones.h` file is:
- **Automatically generated** during build
- **Git-ignored** (not committed to the repository)
- **API-compatible** with the previous static list

## Data Format

The source CSV has the format:
```csv
"Africa/Abidjan","GMT0"
"Europe/Paris","CET-1CEST,M3.5.0,M10.5.0/3"
"America/New_York","EST5EDT,M3.2.0,M11.1.0"
```

The generated C++ structure:
```cpp
namespace timezones {
    struct Timezone {
        const char* name;        // e.g., "Paris"
        const char* posixString; // e.g., "CET-1CEST,M3.5.0,M10.5.0/3"
    };
    
    struct Continent {
        const char* name;
        const Timezone* timezones;
        int count;
    };
    
    static const Timezone EUROPE[] = { ... };
    static const Continent CONTINENTS[] = { ... };
}
```
