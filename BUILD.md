# Building ProofingChamber2 Firmware

This document provides comprehensive instructions for building the ProofingChamber2 ESP32 firmware.

## Prerequisites

- **Python 3.6 or later**: Required for PlatformIO
- **pip**: Python package installer
- **Internet connection**: Required for first build (to download platform packages and libraries)

## Quick Build

For agents or automated systems, the easiest way to build is:

```bash
./build.sh
```

This script handles everything automatically.

## Build Methods

### Method 1: Build Script (Recommended for Automation)

The `build.sh` script provides a simple, automated way to build the project:

```bash
./build.sh
```

**What it does:**
1. Checks if PlatformIO is installed
2. Installs PlatformIO if needed (using `pip3 install --user platformio`)
3. Builds the firmware
4. Reports success or failure with helpful error messages

**Exit codes:**
- `0`: Build successful
- `Non-zero`: Build failed (check output for details)

### Method 2: Using Make

A `Makefile` is provided with convenient targets:

```bash
# Show all available targets
make help

# Install PlatformIO (if not already installed)
make install

# Build the firmware
make build

# Install PlatformIO and build in one step
make all

# Clean build artifacts
make clean

# Upload to device (requires connected hardware)
make upload

# Open serial monitor
make monitor
```

### Method 3: Using PlatformIO CLI Directly

If you prefer to use PlatformIO directly:

```bash
# Install PlatformIO (one-time setup)
pip3 install --user platformio

# Navigate to firmware directory
cd firmware

# Build
pio run

# Clean
pio run --target clean

# Upload (requires connected hardware)
pio run --target upload

# Monitor serial output
pio device monitor
```

## Build Output

Successful builds produce:

- **Binary file**: `firmware/.pio/build/esp32/firmware.bin`
- **ELF file**: `firmware/.pio/build/esp32/firmware.elf`
- **Build artifacts**: In `firmware/.pio/build/esp32/`

## Continuous Integration

A GitHub Actions workflow is provided in `.github/workflows/build.yml` that:

1. Automatically builds on push to `main` or `develop` branches
2. Builds on pull requests
3. Can be triggered manually via workflow dispatch
4. Uploads firmware artifacts for download

## Troubleshooting

### PlatformIO Installation Fails

**Problem:** `pip install platformio` fails

**Solutions:**
- Try: `pip3 install --user platformio`
- Ensure Python 3.6+ is installed: `python3 --version`
- Check pip is working: `pip3 --version`

### Network Errors During Build

**Problem:** `HTTPClientError` or download failures

**Cause:** PlatformIO needs to download:
- ESP32 platform packages (first time)
- Library dependencies (first time)

**Solutions:**
- Ensure you have internet connectivity
- Check firewall/proxy settings
- Try again (sometimes temporary network issues occur)

### Platform espressif32 Not Found

**Problem:** PlatformIO can't find the ESP32 platform

**Solution:** The first build will download the platform automatically. If it fails:

```bash
cd firmware
pio pkg install --platform espressif32
```

### Build Errors in Source Code

**Problem:** Compilation errors in `.cpp` or `.h` files

**Solution:** Check the error output carefully. Common issues:
- Missing library dependencies (check `platformio.ini`)
- Syntax errors in code
- Incompatible library versions

### Permission Denied Errors

**Problem:** Can't write to `.pio` directory or install PlatformIO

**Solution:**
- Use `--user` flag with pip: `pip3 install --user platformio`
- Check directory permissions
- On Linux/Mac, avoid using `sudo` with pip

## Build Environment

The project is configured for:

- **Platform**: Espressif ESP32 (espressif32)
- **Board**: ESP32-C3-DevKitM-1
- **Framework**: Arduino
- **Monitor Speed**: 115200 baud

Configuration details are in `firmware/platformio.ini`.

## For Agents and CI Systems

When building in an automated environment:

1. **Use the build script**: `./build.sh` handles setup automatically
2. **Check exit codes**: Non-zero exit code indicates failure
3. **First build is slower**: Platform and library downloads take time
4. **Cache PlatformIO files**: For faster CI, cache `~/.platformio/` directory
5. **Internet required**: First build requires internet access

Example for GitHub Actions (see `.github/workflows/build.yml`):

```yaml
- name: Install PlatformIO
  run: pip install --upgrade platformio

- name: Build firmware
  run: |
    cd firmware
    pio run
```

## Additional Resources

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32-C3 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [Project README](README.md)
- [Firmware Details](firmware/README.md)
