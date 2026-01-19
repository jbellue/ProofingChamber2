# Timezone Generation

This directory contains the timezone database and generation scripts for the Proofing Chamber.

## Overview

The timezone list is now automatically generated from the [posix_tz_db](https://github.com/nayarsystems/posix_tz_db) repository, which provides a comprehensive database of POSIX timezone strings.

The generator uses zones.json and creates a **single flat array** of all timezones with continent information embedded in each entry. Much simpler than the old multi-array approach!

## Files

- `posix_tz_db/` - Git submodule containing the timezone database (zones.json)
- `generate_timezones.py` - Python script that parses zones.json and generates `src/Timezones.h`
- `pre_build.py` - PlatformIO extra script that runs the generator before each build

## How it Works

1. **Build-time Generation**: When you build the project with PlatformIO, the `pre_build.py` script automatically runs `generate_timezones.py`
2. **JSON Parsing**: The generator reads `posix_tz_db/zones.json` which contains 461 timezones as a simple key-value dictionary
3. **Smart Regeneration**: Only regenerates if:
   - The submodule commit hash has changed
   - The zones.json file hash has changed
   - The existing Timezones.h doesn't exist or has no metadata
4. **Header Generation**: Creates `src/Timezones.h` with:
   - Metadata comments (commit hash, JSON hash, generation time, timezone count)
   - Single flat array of timezones (sorted alphabetically)
   - Helper functions for querying by continent
   - Default timezone set to Europe/Paris

## Structure

The generated header contains:

### Timezones.h (Static Data Only)
```cpp
namespace timezones {
    struct Timezone {
        const char* continent;   // e.g., "Europe"
        const char* name;        // e.g., "Paris"
        const char* posixString; // e.g., "CET-1CEST,M3.5.0,M10.5.0/3"
    };
    
    static const Timezone TIMEZONES[] = { ... };  // 461 entries
    static const int TIMEZONE_COUNT;
    static const int DEFAULT_TIMEZONE_INDEX;      // Points to Europe/Paris
}
```

### TimezoneHelpers.h (Helper Functions)
```cpp
namespace timezones {
    // Helper functions for working with timezone data
    int getContinentCount();
    const char* getContinentName(int continentIndex);
    int getTimezoneCount(const char* continent);
    const Timezone* getTimezone(const char* continent, int localIndex);
    int findTimezoneIndex(const char* posixString);
}
```

To use the helper functions, include both headers:
```cpp
#include "Timezones.h"
#include "TimezoneHelpers.h"
```

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

## Generated Files

### src/Timezones.h
- **Automatically generated** during build
- **Git-ignored** (not committed to the repository)
- **Contains only static data** - timezone array, struct definition, constants
- Clean and simple - just the data

### src/TimezoneHelpers.h
- **Manually maintained** helper file
- Contains inline functions for convenient timezone access
- Includes `Timezones.h` internally

## Metadata

The generated file includes metadata comments at the top:
- Source repository URL
- Submodule commit hash
- JSON file hash (SHA256)
- Generation timestamp
- Total timezone count
- Continent count

This metadata is used to determine if regeneration is needed.
