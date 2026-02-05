cmake_minimum_required(VERSION 3.14)
project(ProofingChamber2Tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Download and configure GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add mocks include directory as SYSTEM to prioritize it
include_directories(BEFORE SYSTEM
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

# Then add source directory
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Define mocks library
add_library(arduino_mocks STATIC
  mocks/Arduino.cpp
  mocks/Preferences.cpp
  mocks/U8g2lib.cpp
  mocks/OneWire.cpp
  mocks/DallasTemperature.cpp
  mocks/WiFiManager.cpp
)

target_include_directories(arduino_mocks PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

# Source files to test
set(SIMPLE_TIME_SOURCES
  ../src/SimpleTime.cpp
)

# Test executables
add_executable(test_simple_time
  test_simple_time.cpp
  ${SIMPLE_TIME_SOURCES}
)

target_link_libraries(test_simple_time
  gtest_main
  arduino_mocks
)

# Note: Storage tests disabled for now due to DebugUtils.h dependency
# that requires ESP-specific headers. To enable, would need to refactor
# Storage.cpp to use dependency injection for logging.

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(test_simple_time)
# gtest_discover_tests(test_storage)  # Disabled

# Add a custom target to run all tests
add_custom_target(run_all_tests
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  DEPENDS test_simple_time
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
