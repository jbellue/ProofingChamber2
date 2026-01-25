# Copilot Instructions for ProofingChamber2

## Project Overview

ProofingChamber2 is an ESP32-based temperature controller for a bread proofing chamber. The system manages both heating and cooling to maintain optimal proofing conditions, with a user interface using an OLED display and rotary encoder for input.

## Technology Stack

- **Platform**: ESP32-C3 (PlatformIO)
- **Framework**: Arduino
- **Display**: SH1106 128x64 OLED (U8G2 library)
- **Temperature Sensor**: DS18B20 (OneWire/Dallas Temperature)
- **Storage**: ESP32 Preferences (NVS)
- **Key Libraries**:
  - U8G2: Display management
  - WiFiManager: WiFi configuration
  - OneWire/DallasTemperature: Temperature sensing
  - rotaryencoder: User input

## Architecture

### Core Components

1. **DisplayManager**: Manages the OLED display using U8G2 library
2. **TemperatureController**: Controls heating and cooling relays based on temperature readings
3. **InputManager**: Handles rotary encoder and button input
4. **ScreensManager**: Manages screen transitions and active screen
5. **Storage**: Static class for Preferences-based storage operations (read/write configuration to NVS)
6. **DS18B20Manager**: Manages DS18B20 temperature sensor

### Design Patterns

- **Screen-based architecture**: All UI screens inherit from `Screen` base class
- **Manager pattern**: Dedicated managers for display, input, screens, and temperature control
- **Static utility class**: Storage class provides static methods for file operations
- **Composition**: Main objects are composed in `main.cpp` and passed to screens as needed

## Coding Conventions

### File Organization

- Header guards: Use `#pragma once` for new files
- Header/implementation pairs: Each class has a `.h` and `.cpp` file
- Screen classes: Located in `src/screens/` directory
- Manager classes: Located in `src/` root directory

### Naming Conventions

- **Classes**: PascalCase (e.g., `DisplayManager`, `TemperatureController`)
- **Methods**: camelCase (e.g., `begin()`, `getMode()`, `setActiveScreen()`)
- **Private members**: Underscore prefix (e.g., `_heaterPin`, `_currentMode`, `_initialized`)
- **Constants**: UPPER_SNAKE_CASE for general constants (e.g., `DS18B20_PIN`), and prefer `static constexpr` for storage keys/defaults in `src/StorageConstants.h`.
- **Parameters**: camelCase (e.g., `heaterPin`, `coolerPin`, `currentTemp`)

### Code Style

- Use `const` for parameters that won't be modified
- Use `const uint8_t` for pin definitions in constructors
- Prefer references (`&`) for object parameters to avoid copying
- Use enum classes or enums within classes for type safety
- Use static class methods for utility functions (see `Storage` class)

### Memory Management

- Prefer stack allocation over heap allocation where possible
- Use `const char*` for string literals
- Be mindful of memory constraints on ESP32

### Debug Logging

- Use `DEBUG_PRINT()` and `DEBUG_PRINTLN()` macros from `DebugUtils.h`
- Debug output is controlled by `DEBUG` flag (set to 1 for debug builds)
- Serial communication is initialized at 115200 baud when debugging

## Pin Definitions

Current pin assignments (defined in `main.cpp`):
- DS18B20_PIN: 0
- COOLING_RELAY_PIN: 1
- HEATING_RELAY_PIN: 2
- ENCODER_CLK: 3
- ENCODER_DT: 4
- ENCODER_SW: 10

Note: The OLED display uses hardware I2C with default ESP32-C3 pins (typically GPIO8 for SDA and GPIO9 for SCL)

## Build and Development

### Building

```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

### Configuration

- Build configuration is in `platformio.ini`
- Board: esp32-c3-devkitm-1
- Storage: ESP32 Preferences (NVS)
- Monitor speed: 115200

### Testing

- This is an embedded project without automated tests
- Testing is done manually on hardware
- Verify functionality through serial debug output when DEBUG is enabled

## Screen Development

When creating new screens:

1. Inherit from `Screen` base class
2. Implement required virtual methods
3. Accept `DisplayManager*` and `InputManager*` in constructor
4. Create both `.h` and `.cpp` files in `src/screens/` directory
5. Add screen to `ScreensManager` in `main.cpp`
6. Wire up screen transitions via `setNextScreen()` or menu actions

## Storage/Persistence

- **Backend**: ESP32 NVS (Preferences) via the static `Storage` class
- **Keys, not paths**: Use preference keys directly (e.g., `hot_lower_limit`) — no path-to-key mapping
- **Constants**: Define keys and defaults in `src/StorageConstants.h` using `static constexpr` (e.g., `HOT_LOWER_LIMIT_KEY`, `HOT_LOWER_LIMIT_DEFAULT`)
- **Interface & DI**: Access storage through `services::IStorage` for decoupling and testability; the concrete `services::StorageAdapter` forwards to `Storage`
- **Initialization**: Call `begin()` once at startup before any read/write
- **Static Storage API**: `getInt(key, default)`, `setInt(key, value)`, `getFloat(key, default)`, `setFloat(key, value)`, `getCharArray(key, buffer, size, default)`, `setCharArray(key, value)`
- **IStorage interface API**: `readInt(key, default)`, `writeInt(key, value)`, `readFloat(key, default)`, `writeFloat(key, value)`, `readString(key, buffer, size, default)`, `writeString(key, value)`
- **Defaults**: Always provide sensible defaults on reads; writes return `true/false` for success
- **Null Object**: `services::NullStorage` provides a safe no-op implementation for testing or uninitialized contexts

Example usage:

```cpp
// Initialization (main.cpp)
services::StorageAdapter storageAdapter;
storageAdapter.begin();
appContext.storage = &storageAdapter;

// Reading values
int hotLower = appContext.storage->getInt(storage::keys::HOT_LOWER_LIMIT_KEY, storage::defaults::HOT_LOWER_LIMIT_DEFAULT);
int coldUpper = appContext.storage->getInt(storage::keys::COLD_UPPER_LIMIT_KEY, storage::defaults::COLD_UPPER_LIMIT_DEFAULT);

// Writing values
bool ok = appContext.storage->setInt(storage::keys::HOT_UPPER_LIMIT_KEY, 32);

// Reading timezone string via IStorage
char tz[64];
appContext.storage->getCharArray(storage::keys::TIMEZONE_KEY, tz, sizeof(tz), storage::defaults::TIMEZONE_DEFAULT);

// Or, using static Storage directly
Storage::getCharArray(storage::keys::TIMEZONE_KEY, tz, sizeof(tz), storage::defaults::TIMEZONE_DEFAULT);
```

## Display Guidelines

- Display resolution: 128x64 pixels
- Use `DisplayManager` wrapper methods instead of direct U8G2 calls
- Common UI elements: `drawTitle()`, `drawButton()`, `drawButtons()`
- Always call `clearBuffer()` before drawing and `sendBuffer()` after
- Font management: Set font before drawing text

## Temperature Control

- Three modes: `HEATING`, `COOLING`, `OFF`
- Temperature limits are loaded from storage
- Controller automatically manages relay states based on current temperature
- Call `update(currentTemp)` in loop to maintain temperature control

## Common Pitfalls

- Don't forget to call `begin()` on managers during setup
- Always initialize Storage before using it
- Remember that ESP32-C3 has limited GPIO pins - plan pin assignments carefully
- Be careful with blocking operations in loop() - keep it responsive
- WiFiManager may block during configuration - handle appropriately
