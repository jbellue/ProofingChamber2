#!/bin/bash
# Test runner script for ProofingChamber2 unit tests
# Builds and runs unit tests using CMake and GoogleTest

set -e

echo "========================================="
echo "ProofingChamber2 Unit Test Runner"
echo "========================================="
echo ""

# Navigate to test directory
cd firmware/test

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "Error: cmake not found. Please install CMake to run tests."
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  macOS: brew install cmake"
    exit 1
fi

echo "CMake version: $(cmake --version | head -1)"
echo ""

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring tests with CMake..."
cmake ..

# Build tests
echo ""
echo "Building tests..."
cmake --build .

# Run tests
echo ""
echo "========================================="
echo "Running Unit Tests"
echo "========================================="
echo ""

# Run with CTest for nice output
ctest --output-on-failure

TEST_EXIT_CODE=$?

echo ""
if [ $TEST_EXIT_CODE -eq 0 ]; then
    echo "========================================="
    echo "✅ ALL TESTS PASSED"
    echo "========================================="
else
    echo "========================================="
    echo "❌ SOME TESTS FAILED"
    echo "========================================="
fi

exit $TEST_EXIT_CODE
