#!/bin/bash
# Run all unit tests using PlatformIO

set -e
cd /home/runner/work/ProofingChamber2/ProofingChamber2

echo "======================================================================"
echo "Running Unit Tests via PlatformIO"
echo "======================================================================"
echo ""

# Run all tests in the native environment using PlatformIO
# PlatformIO will automatically download Unity and ArduinoFake from lib_deps
python3 -m platformio test -e native -v

echo ""
echo "======================================================================"
echo "All Tests Completed!"
echo "======================================================================"



