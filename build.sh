#!/bin/bash
# Build script for ProofingChamber2 firmware
# This script installs PlatformIO if needed and builds the firmware
# 
# Usage:
#   ./build.sh           - Full build (requires internet for first build)
#   ./build.sh --check   - Syntax check only (works offline)

# Check for flags
CHECK_ONLY=false
if [ "$1" == "--check" ] || [ "$1" == "--check-syntax" ]; then
    CHECK_ONLY=true
fi

if [ "$CHECK_ONLY" = true ]; then
    # Run syntax check only - doesn't require platform downloads
    if [ -f "check-syntax.sh" ]; then
        ./check-syntax.sh
        exit $?
    else
        echo "Error: check-syntax.sh not found"
        exit 1
    fi
fi

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
    echo "WORKAROUND: If you're in a restricted environment without internet,"
    echo "you can run syntax checking instead:"
    echo "  ./build.sh --check"
    echo ""
    echo "For verbose output, try: pio run -v (in firmware/ directory)"
    echo "==================================="
    exit $BUILD_EXIT_CODE
fi
