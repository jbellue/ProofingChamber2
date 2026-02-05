#!/bin/bash
# Build script for ProofingChamber2 firmware
# This script installs PlatformIO if needed and builds the firmware

echo "==================================="
echo "ProofingChamber2 Build Script"
echo "==================================="

# Check if PlatformIO is installed
if ! command -v pio &> /dev/null; then
    echo "PlatformIO not found. Installing..."
    pip3 install --user platformio
    
    # Add to PATH if not already there
    export PATH="$PATH:$HOME/.local/bin"
    
    echo "PlatformIO installed successfully!"
else
    echo "PlatformIO is already installed."
    pio --version
fi

# Navigate to firmware directory
cd firmware

echo ""
echo "Building firmware..."
echo ""

# Build the project - note: set -e is NOT used so we can capture errors
if pio run; then
    echo ""
    echo "==================================="
    echo "Build completed successfully!"
    echo "==================================="
    exit 0
else
    BUILD_EXIT_CODE=$?
    echo ""
    echo "==================================="
    echo "Build failed with exit code: $BUILD_EXIT_CODE"
    echo ""
    echo "Common issues:"
    echo "  - Network connectivity (PlatformIO needs to download platform packages)"
    echo "  - Missing dependencies"
    echo "  - Configuration errors in platformio.ini"
    echo ""
    echo "Try running 'pio run -v' in the firmware/ directory for verbose output."
    echo "==================================="
    exit $BUILD_EXIT_CODE
fi
