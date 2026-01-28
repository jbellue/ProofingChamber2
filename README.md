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

### Firmware

1. Install PlatformIO (VS Code extension or CLI)
2. Build from the firmware folder:

```
cd firmware
pio run
```

### Hardware Vendor References

If the repository was cloned without submodules, fetch them:

```
git submodule update --init --recursive
```

## Detailed Docs

- See firmware details in firmware/README.md
- Hardware-specific docs live under hardware/ (or board project folders)
