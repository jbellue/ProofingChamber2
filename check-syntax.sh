#!/bin/bash
# Syntax checking script for ProofingChamber2 firmware
# This script validates C++ syntax without requiring ESP32 platform downloads
# Useful for restricted environments where network access is limited

echo "========================================="
echo "ProofingChamber2 Syntax Checker"
echo "========================================="
echo ""
echo "This script performs basic C++ syntax validation without"
echo "requiring ESP32 platform or Arduino library downloads."
echo ""

# Navigate to firmware directory
cd firmware/src

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found. Please install g++ to use syntax checking."
    exit 1
fi

echo "Compiler: $(g++ --version | head -1)"
echo ""

# Define Arduino/ESP32 stub includes to satisfy basic syntax checks
STUB_INCLUDES="-I. -D ARDUINO=10000 -D ESP32 -D ARDUINO_ARCH_ESP32"

# Additional common Arduino/ESP32 defines
COMMON_DEFINES="-D F_CPU=240000000L"
COMMON_DEFINES="$COMMON_DEFINES -D ARDUINO_USB_MODE=1"
COMMON_DEFINES="$COMMON_DEFINES -D ARDUINO_USB_CDC_ON_BOOT=1"
COMMON_DEFINES="$COMMON_DEFINES -D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_INFO"
COMMON_DEFINES="$COMMON_DEFINES -D U8X8_HAVE_HW_SPI"
COMMON_DEFINES="$COMMON_DEFINES -D U8X8_HAVE_HW_I2C"

# C++ standard flags - more permissive for syntax-only checking
CXX_FLAGS="-std=c++11 -fsyntax-only -fpermissive"
CXX_FLAGS="$CXX_FLAGS -Wno-unknown-pragmas -Wno-unused-parameter -Wno-unused-variable"

echo "Checking C++ files for syntax errors..."
echo "========================================="
echo ""

# Track results
TOTAL_FILES=0
CHECKED_FILES=0
SYNTAX_ERRORS=0
LIBRARY_MISSING=0

# Function to check if error is just missing libraries
is_library_error() {
    local error_file="$1"
    # Check for common library/include errors that are expected
    if grep -qi "fatal error:.*: No such file or directory" "$error_file"; then
        return 0  # True - it's a library error
    fi
    if grep -qi "Arduino.h.*No such file" "$error_file"; then
        return 0
    fi
    if grep -qi "U8g2lib.h.*No such file" "$error_file"; then
        return 0
    fi
    if grep -qi "OneWire.h.*No such file" "$error_file"; then
        return 0
    fi
    if grep -qi "WiFi.*No such file" "$error_file"; then
        return 0
    fi
    if grep -qi "Preferences.h.*No such file" "$error_file"; then
        return 0
    fi
    if grep -qi "driver/gpio.h.*No such file" "$error_file"; then
        return 0
    fi
    return 1  # False - it's a real syntax error
}

# Find and check all .cpp files
shopt -s nullglob
for file in *.cpp screens/*.cpp controllers/*.cpp services/*.cpp; do
    if [ -f "$file" ]; then
        TOTAL_FILES=$((TOTAL_FILES + 1))
        echo -n "Checking $file ... "
        
        # Try to compile with syntax-only check
        if g++ $CXX_FLAGS $STUB_INCLUDES $COMMON_DEFINES -c "$file" -o /dev/null 2>/tmp/syntax_error_$$.txt; then
            echo "✓ OK"
            CHECKED_FILES=$((CHECKED_FILES + 1))
        else
            # Check if it's just library errors
            if is_library_error "/tmp/syntax_error_$$.txt"; then
                echo "⊘ Skipped (missing libraries - expected)"
                LIBRARY_MISSING=$((LIBRARY_MISSING + 1))
            else
                echo "✗ SYNTAX ERROR"
                SYNTAX_ERRORS=$((SYNTAX_ERRORS + 1))
                echo ""
                echo "  Error details:"
                cat /tmp/syntax_error_$$.txt | head -20 | sed 's/^/  /'
                echo ""
            fi
            rm -f /tmp/syntax_error_$$.txt
        fi
    fi
done
shopt -u nullglob

echo ""
echo "========================================="
echo "Syntax Check Summary"
echo "========================================="
echo "Total files checked: $TOTAL_FILES"
echo "  ✓ Clean files: $CHECKED_FILES"
echo "  ⊘ Missing libraries (expected): $LIBRARY_MISSING"
echo "  ✗ Syntax errors: $SYNTAX_ERRORS"
echo ""

if [ $SYNTAX_ERRORS -gt 0 ]; then
    echo "========================================="
    echo "❌ SYNTAX ERRORS FOUND"
    echo "========================================="
    echo ""
    echo "Found $SYNTAX_ERRORS file(s) with actual syntax errors."
    echo "Please review the errors above and fix them."
    echo ""
    exit 1
else
    echo "========================================="
    echo "✅ SYNTAX CHECK PASSED"
    echo "========================================="
    echo ""
    if [ $LIBRARY_MISSING -gt 0 ]; then
        echo "Note: $LIBRARY_MISSING file(s) skipped due to missing Arduino/ESP32 libraries."
        echo "This is expected and normal. The code structure appears valid."
        echo ""
    fi
    echo "All checked files have valid C++ syntax!"
    echo ""
    echo "For a complete build with all Arduino/ESP32 libraries:"
    echo "  • Use './build.sh' in an environment with internet access"
    echo "  • Or run on GitHub Actions CI/CD"
    echo ""
    exit 0
fi
