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
- **Constants/Defines**: UPPER_SNAKE_CASE (e.g., `DS18B20_PIN`, `HEATING_RELAY_PIN`)
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

- Use `Storage` class static methods for all storage operations
- Data is stored in ESP32 NVS (Non-Volatile Storage) using Preferences library
- File paths are automatically converted to preference keys (e.g., `/hot/lower_limit.txt` → `hot_lower_limit`)
- Available methods: `readIntFromFile()`, `writeIntToFile()`, `readFloatFromFile()`, `writeFloatToFile()`, `readStringFromFile()`, `writeStringToFile()`
- Always provide default values when reading
- Check return values when writing

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
