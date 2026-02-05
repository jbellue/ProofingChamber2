# Makefile for ProofingChamber2
# Provides convenient targets for building and managing the firmware

.PHONY: help install build clean upload monitor check all

# Default target
help:
	@echo "ProofingChamber2 Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make install  - Install PlatformIO if not already installed"
	@echo "  make build    - Build the firmware (requires internet)"
	@echo "  make check    - Check syntax only (works offline)"
	@echo "  make clean    - Clean build artifacts"
	@echo "  make upload   - Upload firmware to device"
	@echo "  make monitor  - Open serial monitor"
	@echo "  make all      - Install dependencies and build"
	@echo ""
	@echo "For restricted environments without internet access:"
	@echo "  make check    - Validates C++ syntax without downloading dependencies"
	@echo ""

# Install PlatformIO
install:
	@echo "Installing PlatformIO..."
	@pip3 install --user platformio || pip install --user platformio
	@echo "PlatformIO installed successfully!"

# Build the firmware
build:
	@echo "Building firmware..."
	@cd firmware && pio run

# Check syntax only (works offline)
check:
	@echo "Checking syntax..."
	@./check-syntax.sh

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@cd firmware && pio run --target clean

# Upload to device
upload:
	@echo "Uploading firmware to device..."
	@cd firmware && pio run --target upload

# Monitor serial output
monitor:
	@echo "Opening serial monitor (115200 baud)..."
	@cd firmware && pio device monitor

# Install and build
all: install build
