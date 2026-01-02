# Unit Tests

This directory contains unit tests for the ProofingChamber2 project that can be run on the host machine (native environment) without requiring ESP32 hardware.

## Test Structure

The tests use the Unity test framework and are organized as follows:

- `test_SimpleTime.cpp` - Tests for the SimpleTime class (time manipulation logic)
- `test_OptionalBool.cpp` - Tests for the OptionalBool utility struct
- `test_SafePtr.cpp` - Tests for the SafePtr null object pattern implementation
- `test_Graph.cpp` - Tests for the Graph class (data graphing logic)

## Mocks

Since some code depends on Arduino and hardware-specific libraries, mock implementations are provided in the `test/` directory:

- `mock_Arduino.h` / `mock_Arduino.cpp` - Mock Arduino functions (pinMode, digitalWrite, etc.)
- `Arduino.h` - Symlink to mock_Arduino.h
- `U8g2lib.h` - Mock U8G2 display library
- `DebugUtils.h` - Mock debug utilities

## Running Tests

### Using the Test Script

The easiest way to run all tests:

```bash
./test/run_tests.sh
```

This script uses PlatformIO to automatically download dependencies (Unity and ArduinoFake) and run all tests.

### Using PlatformIO Directly

You can also run tests directly with PlatformIO:

```bash
# Run all tests
pio test -e native

# Run with verbose output
pio test -e native -v

# Run specific test
pio test -e native -f test_SimpleTime
```

PlatformIO will automatically download and manage the test dependencies defined in `platformio.ini`:
- `throwtheswitch/Unity@^2.6.0` - C unit testing framework
- `fabiobatsilva/ArduinoFake@^0.4.0` - Arduino mocking framework

## Test Coverage

The tests cover:

### SimpleTime (26 tests)
- Constructor initialization
- Hour increment/decrement with rollover logic
- Minute increment/decrement with rollover logic
- All comparison operators (==, !=, <, >, <=, >=)
- Edge cases (boundary values, rollovers)

### OptionalBool (17 tests)
- Constructor behavior for all states (Unset, True, False)
- State checking (isSet)
- Value retrieval (getValue)
- Equality and inequality operators
- State transitions

### SafePtr (6 tests)
- Null pointer detection
- Safe pointer resolution using Null Object Pattern
- Singleton behavior of null objects
- Multiple resolve operations

### Graph (17 tests)
- Configuration with various dimensions
- Adding data points
- Circular buffer behavior
- Averaging functionality
- Clear and reset operations
- Edge cases (negative values, out of range values)

## What's Not Covered

The following components are harder to test on the host due to deep hardware dependencies:

- **TemperatureController** - While the logic is testable, it requires extensive mocking of Arduino GPIO functions and debug output. The hysteresis control logic is well-documented in the code comments.
- **DS18B20Manager** - Depends on OneWire and DallasTemperature libraries
- **DisplayManager** - Depends on U8G2 display hardware
- **InputManager** - Depends on rotary encoder hardware
- **NetworkService** - Depends on WiFi and NTP
- **StorageAdapter** - Depends on LittleFS filesystem

These components are better tested through integration tests on actual hardware or could be unit tested with more sophisticated mocking frameworks (e.g., GoogleTest with GMock).

## Requirements

- PlatformIO (will automatically download dependencies)
- GCC/G++ compiler with C++11 support (for native platform)

Dependencies are automatically managed by PlatformIO via `lib_deps` in `platformio.ini`:
- Unity 2.6.0+ (testing framework)
- ArduinoFake 0.4.0+ (Arduino mocking framework)

## Future Improvements

1. Add more mocking infrastructure to test TemperatureController
2. Consider using GoogleTest/GMock for more sophisticated mocking
3. Add continuous integration to run tests automatically
4. Measure code coverage
5. Add integration tests for hardware-dependent components
