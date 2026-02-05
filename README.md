# Proofing Chamber 2

A smart, temperature-controlled proofing chamber project.

## Repository Structure

- firmware/ — ESP32 firmware (PlatformIO)
- hardware/ — KiCad designs and vendor references
  - boards/ — device PCB projects
  - libraries/ — project symbols/footprints
  - docs/ — exports (PDFs, BOMs)
  - third_party/ — external references (git submodules)

## Quick Start

### Building the Firmware

There are several ways to build the firmware:

#### Option 1: Using the Build Script (Recommended for Automation)

```bash
./build.sh
```

This script will:
- Install PlatformIO if not already present
- Build the firmware automatically
- Provide clear error messages if something goes wrong

**For Restricted Environments (No Internet):**
```bash
./build.sh --check  # Syntax checking only
```

### Running Unit Tests

The firmware includes unit tests with mocked Arduino/ESP32 libraries:

```bash
./run-tests.sh
# or
make test
```

Tests run completely offline and don't require hardware or the ESP32 platform. See `firmware/test/README.md` for details.

This performs syntax checking without downloading ESP32 platform packages.

#### Option 2: Using Make

```bash
make all      # Install PlatformIO and build
make build    # Build only (assumes PlatformIO is installed)
make check    # Syntax check only (works offline)
make clean    # Clean build artifacts
```

Run `make help` for all available targets.

#### Option 3: Using PlatformIO Directly

```bash
cd firmware
pio run
```

**Prerequisites:**
- Python 3.6 or later
- pip (Python package installer)
- Internet connection (for first build to download platform packages and dependencies)

**Note:** The first build will take longer as PlatformIO downloads the ESP32 platform and required libraries.

### Workaround for Restricted Environments

If you're in an environment where internet access is restricted (e.g., some CI systems, air-gapped networks), you can use the **syntax checking** feature:

```bash
# Using build script
./build.sh --check

# Or using make
make check
```

This validates C++ syntax without requiring ESP32 platform downloads. While it can't produce a firmware binary, it verifies code structure and catches syntax errors.

### Hardware Vendor References

If the repository was cloned without submodules, fetch them:

```
git submodule update --init --recursive
```

## Detailed Docs

- See firmware details in firmware/README.md
- Hardware-specific docs live under hardware/ (or board project folders)
