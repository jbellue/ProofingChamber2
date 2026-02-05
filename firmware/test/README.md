# Unit Tests

This directory contains unit tests for the ProofingChamber2 firmware using GoogleTest framework with mocked Arduino/ESP32 libraries.

## Overview

The unit tests allow testing business logic without requiring actual hardware or the ESP32 platform. This is achieved through:

- **Mock Libraries**: Full mock implementations of Arduino, ESP32, and third-party libraries
- **GoogleTest**: Industry-standard C++ testing framework  
- **CMake**: Cross-platform build system
- **No Hardware Required**: Tests run on any system with C++ compiler

## Running Tests

### Quick Start

From the project root:

```bash
./run-tests.sh
```

Or using Make:

```bash
make test
```

### Manual Build

```bash
cd firmware/test
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

## Test Structure

```
test/
├── CMakeLists.txt          # CMake configuration
├── mocks/                  # Mock library implementations
│   ├── Arduino.h/cpp       # Arduino core mocks
│   ├── Preferences.h/cpp   # ESP32 NVS storage mocks
│   ├── U8g2lib.h/cpp      # Display library mocks
│   ├── OneWire.h/cpp      # OneWire protocol mocks
│   ├── DallasTemperature.h/cpp  # Temperature sensor mocks
│   ├── WiFiManager.h/cpp  # WiFi management mocks
│   └── DebugUtils.h       # Debug logging mocks
├── test_simple_time.cpp   # SimpleTime class tests
└── test_storage.cpp       # Storage class tests (disabled)
```

## Mocked Libraries

### Arduino Core
- `pinMode`, `digitalWrite`, `digitalRead`
- `millis`, `micros`, `delay`
- `Serial` class
- `String` class

### ESP32 Libraries
- `Preferences` - NVS storage with in-memory implementation
- GPIO functions

### Third-Party Libraries
- `U8g2` - Display operations (no-op mocks)
- `OneWire` / `DallasTemperature` - Temperature sensor (returns mock data)
- `WiFiManager` - Network configuration (returns success)

## Current Tests

### SimpleTime Tests (19 tests)
Tests for the `SimpleTime` class which handles time manipulation:

- Constructor tests
- Increment/decrement operations
- Overflow/underflow handling
- Comparison operators
- Edge cases with day rollovers

**Status**: ✅ All passing

### Storage Tests
Tests for the `Storage` class (persistent settings):

**Status**: ⚠️ Disabled - requires refactoring to remove direct ESP32 dependencies

## Writing New Tests

### Example Test

```cpp
#include <gtest/gtest.h>
#include "YourClass.h"

class YourClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup before each test
    }
    
    void TearDown() override {
        // Cleanup after each test
    }
};

TEST_F(YourClassTest, TestSomething) {
    YourClass obj;
    EXPECT_EQ(obj.getValue(), 42);
}
```

### Adding a New Test File

1. Create `test_yourclass.cpp` in this directory
2. Add to `CMakeLists.txt`:
   ```cmake
   add_executable(test_yourclass
     test_yourclass.cpp
     ../src/YourClass.cpp
   )
   target_link_libraries(test_yourclass gtest_main arduino_mocks)
   gtest_discover_tests(test_yourclass)
   ```

## Prerequisites

- **CMake** 3.14 or higher
- **C++ Compiler** with C++11 support (g++, clang++)
- **Internet** (first run only, to download GoogleTest)

### Installing CMake

Ubuntu/Debian:
```bash
sudo apt-get install cmake
```

macOS:
```bash
brew install cmake
```

## Benefits

✅ **Fast Feedback**: Tests run in milliseconds  
✅ **No Hardware**: Test on any development machine  
✅ **CI-Friendly**: Easy integration with CI/CD pipelines  
✅ **Regression Prevention**: Catch bugs before they reach hardware  
✅ **Refactoring Safety**: Confidently refactor with test coverage  
✅ **Documentation**: Tests serve as usage examples

## Limitations

❌ Cannot test hardware-specific behavior  
❌ Cannot test actual Arduino/ESP32 library integration  
❌ Cannot test timing-critical code accurately  
❌ Some classes require refactoring for testability

## Future Improvements

- [ ] Add more unit tests for other classes
- [ ] Refactor Storage to enable testing
- [ ] Add integration tests
- [ ] Increase test coverage
- [ ] Add CI/CD integration for automatic test runs

## Troubleshooting

### CMake Not Found
Install CMake using your package manager (see Prerequisites)

### GoogleTest Download Fails
Check internet connectivity. GoogleTest is downloaded automatically on first build.

### Tests Won't Compile
Ensure you're using C++11 or later:
```bash
g++ --version  # Should be 4.8 or later
```

### Mock Header Not Found
Make sure mocks directory is in include path. Check CMakeLists.txt configuration.

## Resources

- [GoogleTest Documentation](https://google.github.io/googletest/)
- [CMake Documentation](https://cmake.org/documentation/)
- [GoogleTest Primer](https://google.github.io/googletest/primer.html)
