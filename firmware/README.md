# Proofing Chamber

A smart temperature-controlled proofing chamber controller, built with ESP32-C3 and a 128x64 OLED display.

## Hardware

### Components

- **Microcontroller**: ESP32-C3-DevKitM-1
- **Display**: SH1106 128x64 OLED (I2C interface)
- **Temperature Sensor**: DS18B20 (1-Wire protocol)
- **Rotary Encoder**: With push-button switch for menu navigation
- **Relays**: 
  - Heating relay (GPIO 2)
  - Cooling relay (GPIO 1)
- **LEDs**:
  - Proofing LED (GPIO 6) - indicates heating mode
  - Cooling LED (GPIO 5) - indicates cooling mode

### Wiring Diagram

```
ESP32-C3
├── GPIO 0  → DS18B20 (1-Wire)
├── GPIO 1  → Cooling Relay
├── GPIO 2  → Heating Relay
├── GPIO 3  → Rotary Encoder CLK
├── GPIO 4  → Rotary Encoder DT
├── GPIO 5  → Cooling LED (cathode to GND)
├── GPIO 6  → Proofing LED (cathode to GND)
├── GPIO 10 → Rotary Encoder SW
├── SDA (GPIO 8)  → SH1106 OLED SDA
├── SCL (GPIO 9)  → SH1106 OLED SCL
└── GND → All grounds connected

Power:
├── 5V → OLED VCC, Relay logic power
├── 3.3V → ESP32 power
└── GND → All grounds
```

## Features

### Temperature Control

- **Heating Mode**: Maintains target temperature using hysteresis control
  - Turns heater ON below lower limit
  - Turns heater OFF above upper limit
- **Cooling Mode**: Maintains target temperature with active cooling
  - Turns cooler ON above upper limit
  - Turns cooler OFF below lower limit

### Menu System

Navigate with rotary encoder, select with push button:

1. **Mettre en pousse** (Start Proofing) - Immediate heating mode
2. **Mettre en froid** (Cooling)
   - Pousser à... (Proof at specific time)
   - Pousser dans... (Proof in X time)
3. **Réglages** (Settings)
   - **Chaud** (Heating Settings)
     - Lower temperature limit
     - Upper temperature limit
   - **Froid** (Cooling Settings)
     - Lower temperature limit
     - Upper temperature limit
   - **Avancés** (Advanced Settings)
     - Données (Display current temperature)
     - Reset du WiFi (Reset WiFi & reboot)
     - Fuseau horaire (Timezone)
     - Redémarrer (Reboot)

### Data Storage

Settings are persisted to ESP32 NVS (Non-Volatile Storage) using the Preferences library:

```
Preference Keys:
  - h_lower
  - h_upper
  - c_lower
  - c_upper
  - timezone
```

Default values:
- **Heating**: Range 28-35°C
- **Cooling**: Range 2-7°C

### Network Features

- WiFi Manager for easy WiFi configuration
- NTP time synchronization (Europe/Paris timezone)
- WiFi reset option available in advanced settings

## Installation & Setup

### Prerequisites

- PlatformIO IDE or VS Code with PlatformIO extension
- Python 3.6+


### First Boot

1. Device will attempt to connect to WiFi via WiFi Manager
2. If no WiFi found, connect to the ESP32's AP and configure WiFi
3. Device will sync time from NTP server
4. Main menu will appear on display

## Usage Guide

### Starting a Proof

1. From main menu, select **Mettre en pousse**
   - Immediately activates heating mode
   - Displays elapsed time and current temperature
   - Shows temperature graph
   - Press button to cancel and return to menu

### Cooling with Delayed Start

1. From main menu, select **Mettre en froid** → **Pousser dans...**
2. Use encoder to set hours and minutes for delay
3. Press button to confirm each field
4. Device will cool until specified time, then switch to heating
5. Proofing screen appears when time reached

### Scheduled Proof

1. From main menu, select **Mettre en froid** → **Pousser à...**
2. Set the time when proofing should start (current time shown by default)
3. Device will cool until that time, then switch to heating

### Adjusting Temperature Limits

1. Navigate to **Réglages** → **Chaud** or **Froid**
2. Select the limit you want to adjust
3. Use encoder to increase/decrease value
4. Press button to confirm and save

### Temperature Graph

- Displays on proofing and cooling screens
- X-axis: Time (most recent on right)
- Y-axis: Temperature scale (configured per mode)
- Updates every 10 seconds

## Development

### Architecture

The project uses a layered architecture:

```
UI Layer (Views)
    ↓
Controllers (Screen Logic)
    ↓
Services (Temperature, Storage, Network, Reboot)
    ↓
Hardware Drivers (InputManager, DisplayManager, DS18B20Manager)
    ↓
Hardware (ESP32 Peripherals)
```

### Key Classes

- `Menu` - Main menu navigation
- `ProofingController` - Heating mode logic
- `CoolingController` - Cooling mode logic
- `TemperatureController` - Relay control & hysteresis
- `StorageAdapter` - Persistent settings
- `NetworkService` - WiFi & NTP
