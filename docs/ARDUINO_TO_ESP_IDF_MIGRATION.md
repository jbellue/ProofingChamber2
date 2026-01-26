# Arduino Framework to ESP-IDF Migration Guide

## Executive Summary

This document provides a comprehensive guide for migrating ProofingChamber2 from the Arduino framework to native ESP-IDF. The project is an ESP32-C3 based temperature controller with OLED display, rotary encoder interface, and WiFi connectivity.

**Total Migration Effort:** 10-12 weeks
**Lines of Code Affected:** ~640 lines
**Current Status:** Partially migrated (already uses some ESP-IDF APIs)

---

## Table of Contents

1. [Overview](#overview)
2. [Current Dependencies](#current-dependencies)
3. [Component-by-Component Migration](#component-by-component-migration)
4. [Migration Strategy](#migration-strategy)
5. [Risks and Mitigation](#risks-and-mitigation)
6. [Testing Plan](#testing-plan)
7. [References](#references)

---

## Overview

### Why Migrate to ESP-IDF?

**Benefits:**
- **Smaller binary size:** Arduino core adds ~50KB overhead
- **Better performance:** Direct hardware access without Arduino abstraction layers
- **More control:** Fine-grained control over FreeRTOS tasks, memory, and peripherals
- **Official support:** ESP-IDF is Espressif's official SDK with better long-term support
- **Advanced features:** Access to ESP-IDF exclusive features (ULP, secure boot, etc.)

**Challenges:**
- **Steeper learning curve:** More low-level code required
- **Library ecosystem:** Fewer ready-to-use libraries compared to Arduino
- **Development time:** Initial migration requires significant effort

### Current State

The codebase is **partially migrated**. Several ESP-IDF APIs are already in use:
- GPIO driver (`driver/gpio.h`)
- Deep sleep (`esp_sleep.h`)
- ISR attributes (`IRAM_ATTR`)
- Direct GPIO access (`gpio_get_level()`, etc.)

---

## Current Dependencies

### 1. U8G2 Display Library ⚠️ **HIGHEST PRIORITY**

**Current Usage:**
```cpp
// DisplayManager.h
#include <U8g2lib.h>
U8G2_SH1106_128X64_NONAME_F_HW_I2C _display;

// DisplayManager.cpp
_display.begin();
_display.drawUTF8(x, y, text);
_display.drawBox(x, y, w, h);
_display.setFont(u8g2_font_t0_11_tf);
_display.sendBuffer();
```

**Files Affected:**
- `src/DisplayManager.h` (43 lines)
- `src/DisplayManager.cpp` (172 lines)
- `src/IDisplayManager.h` (interface)
- `src/icons.h` (icon definitions)
- All screen view classes (indirect)

**ESP-IDF Alternatives:**

#### Option A: LVGL (Light and Versatile Graphics Library) ⭐ **RECOMMENDED**

**Pros:**
- Feature-rich GUI library with widgets, animations, themes
- Active community and extensive documentation
- Hardware acceleration support (for future ESP32-S3 migration)
- SH1106 driver available
- Font rendering with UTF-8 support

**Cons:**
- Heavier memory footprint than bare-metal approach
- Steeper learning curve
- Different API paradigm (object/widget based vs. immediate mode)

**Implementation:**
```c
// Using LVGL with SH1106 driver
#include "lvgl.h"
#include "lv_port_disp.h"

// Initialize
lv_init();
lv_port_disp_init();

// Draw text
lv_obj_t *label = lv_label_create(lv_scr_act());
lv_label_set_text(label, "Hello");
lv_obj_align(label, LV_ALIGN_CENTER, x, y);

// Buttons
lv_obj_t *btn = lv_btn_create(lv_scr_act());
lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
```

#### Option B: esp_lcd with Custom Graphics

**Pros:**
- Minimal overhead
- Direct control over every pixel
- Official ESP-IDF component

**Cons:**
- Need to implement all drawing primitives manually
- Font rendering is complex
- No ready-made widgets

**Implementation:**
```c
#include "driver/i2c.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

// Initialize I2C and SH1106
esp_lcd_i2c_bus_handle_t i2c_bus = NULL;
esp_lcd_panel_handle_t panel_handle = NULL;
esp_lcd_panel_io_handle_t io_handle = NULL;

// Draw (low-level)
esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 128, 64, buffer);
```

#### Option C: U8G2 without Arduino

**Pros:**
- Keep existing code mostly intact
- U8G2 has ESP-IDF support via `u8g2_esp32_hal`

**Cons:**
- Still depends on external library
- Defeats the purpose of full migration

---

### 2. RotaryEncoder Library ✅ **EASY MIGRATION**

**Current Usage:**
```cpp
// InputManager.h
#include <RotaryEncoder.h>
RotaryEncoder _encoder;

// InputManager.cpp
_encoder(clkPin, dtPin, RotaryEncoder::LatchMode::FOUR3);
long pos = _encoder.getPosition();
_encoder.tick(s1, s2);  // Called in ISR
```

**Files Affected:**
- `src/InputManager.h` (42 lines)
- `src/InputManager.cpp` (123 lines)

**ESP-IDF Solution:**

The project **already uses ESP-IDF GPIO ISR**! Just need to replace the `RotaryEncoder::tick()` logic with a custom state machine.

**Implementation:**
```c
// Custom rotary encoder decoder (FOUR3 latch mode)
typedef struct {
    int8_t position;
    uint8_t last_state;
} rotary_encoder_t;

// State transition table for quadrature encoding
static const int8_t ENCODER_STATES[] = {
    0, -1, 1, 0,   // 0b00
    1, 0, 0, -1,   // 0b01
    -1, 0, 0, 1,   // 0b10
    0, 1, -1, 0    // 0b11
};

void IRAM_ATTR rotary_encoder_tick(rotary_encoder_t *enc, int clk, int dt) {
    uint8_t current_state = (clk << 1) | dt;
    uint8_t index = (enc->last_state << 2) | current_state;
    enc->position += ENCODER_STATES[index];
    enc->last_state = current_state;
}

// In GPIO ISR
static void IRAM_ATTR gpio_isr_handler(void *arg) {
    encoder_context_t *ctx = (encoder_context_t *)arg;
    int clk = gpio_get_level(ctx->clk_pin);
    int dt = gpio_get_level(ctx->dt_pin);
    rotary_encoder_tick(&ctx->encoder, clk, dt);
}
```

**Migration Steps:**
1. Create `rotary_encoder.h/c` with state machine logic (~80 lines)
2. Replace `RotaryEncoder` class with custom struct
3. Update `InputManager` to use new implementation
4. Test with hardware encoder

**Estimated Time:** 1 week
**Risk Level:** LOW (logic is straightforward, hardware already works)

---

### 3. WiFiManager Library ⚠️ **COMPLEX MIGRATION**

**Current Usage:**
```cpp
// NetworkService.cpp
#include <WiFiManager.h>
#include <WiFi.h>

WiFiManager wifiManager;
wifiManager.setConnectTimeout(20);
wifiManager.setConfigPortalTimeout(60);
wifiManager.autoConnect(ssid);
```

**Files Affected:**
- `src/services/NetworkService.h` (13 lines)
- `src/services/NetworkService.cpp` (46 lines)
- `src/screens/Initialization.cpp` (uses captive portal callback)

**ESP-IDF Alternatives:**

#### Option A: esp-idf-provisioning ⭐ **RECOMMENDED**

**Pros:**
- Official ESP-IDF component
- BLE and SoftAP provisioning modes
- Mobile app support (iOS/Android)
- Secure credential handling

**Cons:**
- Different UX than WiFiManager's web portal
- Requires mobile app or custom web interface

**Implementation:**
```c
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_softap.h"

// Initialize provisioning
wifi_prov_mgr_config_t config = {
    .scheme = wifi_prov_scheme_softap,
    .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE
};
wifi_prov_mgr_init(config);

// Start provisioning AP
wifi_prov_mgr_start_provisioning(WIFI_PROV_SECURITY_1, 
                                  proof_of_possession, 
                                  service_name, 
                                  service_key);

// Register callback
wifi_prov_mgr_set_app_cb(prov_event_handler);
```

#### Option B: Custom Captive Portal

**Pros:**
- Full control over UI/UX
- Similar experience to WiFiManager

**Cons:**
- Need to implement HTTP server, DNS server, and credential storage
- More code to maintain (~300-400 lines)

**Implementation:**
```c
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "lwip/apps/netbiosns.h"

// Start SoftAP
esp_wifi_set_mode(WIFI_MODE_AP);
wifi_config_t ap_config = {
    .ap = {
        .ssid = "ProofingChamber",
        .ssid_len = 15,
        .max_connection = 1,
        .authmode = WIFI_AUTH_OPEN
    }
};
esp_wifi_set_config(WIFI_IF_AP, &ap_config);

// Start HTTP server
httpd_config_t config = HTTPD_DEFAULT_CONFIG();
httpd_handle_t server = NULL;
httpd_start(&server, &config);

// Register handlers
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler
};
httpd_register_uri_handler(server, &uri_get);
```

**Migration Steps:**
1. Choose between esp-idf-provisioning (easier) or custom portal (more control)
2. Update `NetworkService` to use chosen approach
3. Update `Initialization` screen to show AP name/QR code
4. Test provisioning flow end-to-end

**Estimated Time:** 3-4 weeks
**Risk Level:** MEDIUM (critical for initial setup, but alternatives exist)

---

### 4. OneWire & DallasTemperature Libraries ✅ **MODERATE MIGRATION**

**Current Usage:**
```cpp
// DS18B20Manager.h
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire _oneWire;
DallasTemperature _sensors;

// DS18B20Manager.cpp
_sensors.begin();
_sensors.requestTemperatures();
_sensors.setResolution(_deviceAddress, bits);
float temp = _sensors.getTempCByIndex(0);
```

**Files Affected:**
- `src/DS18B20Manager.h` (42 lines)
- `src/DS18B20Manager.cpp` (130 lines)

**ESP-IDF Alternative:**

#### OneWire Bus Library (esp-idf-owb) ⭐ **RECOMMENDED**

**Pros:**
- Direct ESP-IDF port of OneWire protocol
- DS18B20 driver available (`ds18b20-esp32`)
- API very similar to Arduino libraries
- Well-maintained

**Implementation:**
```c
#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"

// Initialize OneWire bus using RMT peripheral
owb_rmt_driver_info rmt_driver_info;
OneWireBus *owb = owb_rmt_initialize(&rmt_driver_info, GPIO_NUM_0, 
                                      RMT_CHANNEL_0, RMT_CHANNEL_1);
owb_use_crc(owb, true);

// Find devices
OneWireBus_ROMCode device_rom_code;
owb_status status = owb_read_rom(owb, &device_rom_code);

// Initialize DS18B20
DS18B20_Info *ds18b20_info = ds18b20_malloc();
ds18b20_init_solo(ds18b20_info, owb);
ds18b20_use_crc(ds18b20_info, true);

// Set resolution
ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION_12_BIT);

// Read temperature (async)
ds18b20_convert_all(owb);
vTaskDelay(pdMS_TO_TICKS(750));  // Wait for conversion
float temp = 0;
ds18b20_read_temp(ds18b20_info, &temp);
```

**Migration Steps:**
1. Add `esp-idf-owb` and `ds18b20` components to project
2. Update `DS18B20Manager` to use new API
3. Map state machine (`WAITING_CONVERSION`, `READING_TEMP`, `ERROR`)
4. Update error handling for disconnected sensor
5. Test temperature readings and resolution changes

**Estimated Time:** 2 weeks
**Risk Level:** LOW (protocol is identical, API is similar)

---

### 5. Arduino Preferences (NVS Wrapper) ✅ **TRIVIAL MIGRATION**

**Current Usage:**
```cpp
// Storage.h
#include <Preferences.h>
Preferences preferences;

// Storage.cpp
preferences.begin("storage", false);
int value = preferences.getInt(key, default);
preferences.putInt(key, value);
bool exists = preferences.isKey(key);
```

**Files Affected:**
- `src/Storage.h` (35 lines)
- `src/Storage.cpp` (144 lines)

**ESP-IDF Solution:**

Direct replacement with NVS API. The Arduino `Preferences` class is literally a thin wrapper around ESP-IDF's NVS.

**Implementation:**
```c
#include "nvs_flash.h"
#include "nvs.h"

// Initialize NVS
esp_err_t err = nvs_flash_init();
if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    err = nvs_flash_init();
}

// Open namespace
nvs_handle_t nvs_handle;
nvs_open("storage", NVS_READWRITE, &nvs_handle);

// Read int
int32_t value = 0;
err = nvs_get_i32(nvs_handle, "h_lower", &value);
if (err == ESP_ERR_NVS_NOT_FOUND) {
    value = 27;  // Default
}

// Write int
nvs_set_i32(nvs_handle, "h_lower", 28);
nvs_commit(nvs_handle);

// Check if key exists
int32_t dummy;
err = nvs_get_i32(nvs_handle, "h_lower", &dummy);
bool exists = (err == ESP_OK);

// Read string
size_t required_size = 0;
nvs_get_str(nvs_handle, "timezone", NULL, &required_size);
char *tz = malloc(required_size);
nvs_get_str(nvs_handle, "timezone", tz, &required_size);

// Close handle
nvs_close(nvs_handle);
```

**Migration Steps:**
1. Replace `#include <Preferences.h>` with `#include "nvs_flash.h"`
2. Update `Storage::begin()` to use `nvs_flash_init()` and `nvs_open()`
3. Replace `getInt()/putInt()` with `nvs_get_i32()/nvs_set_i32()`
4. Replace `getString()/putString()` with `nvs_get_str()/nvs_set_str()`
5. Replace `isKey()` with `nvs_get_*() == ESP_OK` check
6. Add `nvs_commit()` after writes
7. Test all storage operations

**Estimated Time:** 2-3 days
**Risk Level:** NONE (direct API mapping)

---

### 6. Arduino Core APIs ✅ **TRIVIAL MIGRATION**

**Current Usage:**

| Arduino API | Usage Count | ESP-IDF Equivalent |
|-------------|-------------|-------------------|
| `Serial.begin(115200)` | 1 | `uart_driver_install()` or `printf()` |
| `Serial.print()/println()` | ~20 (via macros) | `printf()` |
| `pinMode()` | 4 | `gpio_set_direction()` |
| `digitalWrite()` | 6 | `gpio_set_level()` |
| `digitalRead()` | 0 | `gpio_get_level()` (already used) |
| `delay()` | 5 | `vTaskDelay(pdMS_TO_TICKS(ms))` |
| `millis()` | 8 | `esp_timer_get_time() / 1000` |
| `constrain()` | 1 | `(x < min) ? min : (x > max) ? max : x` |
| `ESP.restart()` | 1 | `esp_restart()` |

**Files Affected:**
- `src/DebugUtils.h` (Serial macros)
- `src/main.cpp` (Serial.begin, pinMode)
- `src/TemperatureController.cpp` (pinMode, digitalWrite)
- `src/InputManager.cpp` (millis)
- `src/DS18B20Manager.cpp` (millis, constrain)
- `src/screens/Initialization.cpp` (millis, delay)
- `src/screens/controllers/*.cpp` (delay)
- `src/services/RebootService.cpp` (ESP.restart)

**Implementation:**

```c
// Replace Serial
#include "esp_log.h"
#define DEBUG_PRINTLN(x) ESP_LOGI(TAG, "%s", x)
#define DEBUG_PRINT(x) ESP_LOGI(TAG, "%s", x)

// Replace pinMode
#include "driver/gpio.h"
gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << GPIO_NUM_1),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};
gpio_config(&io_conf);

// Replace digitalWrite
gpio_set_level(GPIO_NUM_1, 1);  // HIGH
gpio_set_level(GPIO_NUM_1, 0);  // LOW

// Replace delay
#include "freertos/task.h"
vTaskDelay(pdMS_TO_TICKS(500));

// Replace millis
#include "esp_timer.h"
uint64_t millis() {
    return esp_timer_get_time() / 1000;
}

// Replace constrain
#define constrain(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// Replace ESP.restart
#include "esp_system.h"
esp_restart();
```

**Migration Steps:**
1. Create `platform_compat.h` with utility macros/functions
2. Update `DebugUtils.h` to use `esp_log.h`
3. Replace Arduino APIs globally (search & replace)
4. Remove `#include <Arduino.h>` from all files
5. Test each subsystem

**Estimated Time:** 3-5 days
**Risk Level:** NONE (straightforward replacements)

---

### 7. WiFi.h APIs

**Current Usage:**
```cpp
#include <WiFi.h>
WiFi.setAutoReconnect(true);
WiFi.persistent(true);
IPAddress ip = WiFi.softAPIP();
```

**ESP-IDF Solution:**
```c
#include "esp_wifi.h"
#include "esp_netif.h"

// Auto-reconnect
esp_wifi_set_auto_connect(true);

// Persistent (NVS storage is automatic)
// No explicit call needed

// Get AP IP
esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
esp_netif_ip_info_t ip_info;
esp_netif_get_ip_info(netif, &ip_info);
```

**Estimated Time:** Included in WiFiManager migration
**Risk Level:** LOW

---

## Migration Strategy

### Recommended Phased Approach

#### Phase 1: Low-Hanging Fruit (Week 1-2)
**Goal:** Replace trivial Arduino APIs with ESP-IDF equivalents

**Tasks:**
1. Create `platform_compat.h` with utility functions
2. Replace Arduino Core APIs (Serial, pinMode, digitalWrite, millis, delay)
3. Migrate Preferences to direct NVS
4. Update build system (platformio.ini → CMakeLists.txt)
5. Test basic functionality

**Deliverables:**
- `platform_compat.h`
- Updated `Storage.cpp` using NVS
- Updated debug macros using `esp_log`

**Risk:** VERY LOW

---

#### Phase 2: Sensor and Input (Week 3-5)
**Goal:** Migrate RotaryEncoder and DS18B20 drivers

**Tasks:**
1. Implement custom rotary encoder state machine
2. Integrate `esp-idf-owb` and `ds18b20` components
3. Update `InputManager` and `DS18B20Manager`
4. Test encoder responsiveness and temperature readings

**Deliverables:**
- `rotary_encoder.c/h`
- Updated `InputManager.cpp`
- Updated `DS18B20Manager.cpp`

**Risk:** LOW

---

#### Phase 3: WiFi Provisioning (Week 6-9)
**Goal:** Replace WiFiManager with ESP-IDF provisioning

**Tasks:**
1. Choose provisioning method (esp-idf-provisioning vs. custom portal)
2. Implement new provisioning flow
3. Update `NetworkService` and `Initialization` screen
4. Add QR code generation for mobile app (if using official provisioning)
5. Test end-to-end provisioning

**Deliverables:**
- Updated `NetworkService.cpp`
- Updated `Initialization.cpp`
- Provisioning test cases

**Risk:** MEDIUM

---

#### Phase 4: Display Migration (Week 10-14)
**Goal:** Replace U8G2 with LVGL or esp_lcd

**Tasks:**
1. Set up LVGL with SH1106 driver
2. Create DisplayManager wrapper around LVGL
3. Migrate all screen views to LVGL widgets
4. Port custom fonts and icons
5. Test all UI screens and transitions
6. Fine-tune performance and memory usage

**Deliverables:**
- Updated `DisplayManager.cpp` using LVGL
- LVGL configuration (`lv_conf.h`)
- All screen views ported
- Performance benchmarks

**Risk:** HIGH (most complex, affects entire UI)

---

#### Phase 5: Integration and Testing (Week 15-16)
**Goal:** System integration and testing

**Tasks:**
1. End-to-end testing of all features
2. Memory profiling and optimization
3. Power consumption testing
4. Long-term stability testing
5. Documentation updates

**Deliverables:**
- Test report
- Performance comparison (Arduino vs. ESP-IDF)
- Updated README and documentation

**Risk:** LOW

---

## Risks and Mitigation

### High Risk: Display Migration

**Risk:** LVGL has a different API paradigm; porting could break UI/UX

**Mitigation:**
- Prototype LVGL integration in separate branch first
- Port one screen at a time, testing each
- Keep `IDisplayManager` interface to minimize changes
- Consider keeping U8G2 with ESP-IDF HAL as fallback

### Medium Risk: WiFiManager Replacement

**Risk:** Users may struggle with new provisioning method

**Mitigation:**
- Thoroughly document new provisioning flow
- Add fallback to hard-coded WiFi credentials for testing
- Provide clear on-screen instructions during provisioning
- Consider implementing both BLE and SoftAP modes

### Low Risk: Timing Changes

**Risk:** Replacing `millis()` could affect timing-sensitive code

**Mitigation:**
- Test encoder ISR thoroughly
- Verify DS18B20 conversion timing
- Add logging to detect timing anomalies

### Low Risk: Memory Leaks

**Risk:** Manual memory management in ESP-IDF could introduce leaks

**Mitigation:**
- Use heap tracing tools (`heap_caps_dump()`)
- Run long-term stability tests
- Follow RAII patterns where possible (use FreeRTOS constructors/destructors)

---

## Testing Plan

### Unit Testing

**Components to Test:**
1. Rotary encoder state machine (simulate GPIO transitions)
2. NVS storage operations (read/write/delete)
3. DS18B20 state machine (mock temperature readings)
4. Display rendering (compare screenshots)

**Tools:**
- ESP-IDF Unity test framework
- Mock GPIO library
- Memory leak detection

### Integration Testing

**Scenarios:**
1. Cold boot with factory reset
2. WiFi provisioning flow
3. Temperature control loop (heating/cooling)
4. Menu navigation with encoder
5. Deep sleep and wake-up
6. OTA firmware update (if implemented)

**Tools:**
- Hardware test rig
- Automated test scripts
- Serial monitor logging

### Performance Testing

**Metrics:**
1. Boot time (target: <5 seconds)
2. Display refresh rate (target: 60 FPS minimum)
3. Encoder responsiveness (target: <10ms latency)
4. Temperature polling rate (target: 10-second interval)
5. Memory usage (target: <80% heap usage)
6. Power consumption (deep sleep: <10µA, active: <150mA)

---

## Build System Migration

### From PlatformIO to ESP-IDF Build System

**Current (`platformio.ini`):**
```ini
[env:esp32]
platform = espressif32
framework = arduino
board = esp32-c3-devkitm-1
lib_deps = 
    tzapu/WiFiManager
    U8g2
    rotaryencoder
    paulstoffregen/OneWire
    milesburton/DallasTemperature
```

**New (`CMakeLists.txt`):**
```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ProofingChamber2)

set(COMPONENT_SRCS
    "src/main.c"
    "src/DisplayManager.c"
    "src/InputManager.c"
    "src/TemperatureController.c"
    "src/Storage.c"
    # ... all other sources
)

set(COMPONENT_ADD_INCLUDEDIRS "src")

set(COMPONENT_REQUIRES
    driver
    nvs_flash
    esp_wifi
    esp_netif
    esp_http_server
    lvgl
    esp-idf-owb
    ds18b20
)

register_component()
```

**Component Dependencies:**
```yaml
dependencies:
  lvgl/lvgl: "^8.3.0"
  espressif/esp-idf-owb: "^1.0.0"
  espressif/ds18b20: "^1.0.0"
```

---

## References

### ESP-IDF Documentation
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [GPIO & RTC GPIO](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/peripherals/gpio.html)
- [NVS (Non-Volatile Storage)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/storage/nvs_flash.html)
- [WiFi Provisioning](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/provisioning/wifi_provisioning.html)

### Libraries
- [LVGL Documentation](https://docs.lvgl.io/)
- [esp-idf-owb GitHub](https://github.com/DavidAntliff/esp-idf-owb)
- [ds18b20 Driver GitHub](https://github.com/DavidAntliff/esp-idf-ds18b20)

### Migration Guides
- [Arduino to ESP-IDF Migration (Espressif)](https://docs.espressif.com/projects/arduino-esp32/en/latest/migration.html)
- [FreeRTOS for Arduino Users](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/01-Task-creation/TaskCreate)

---

## Appendix: Code Migration Checklist

### Pre-Migration
- [ ] Back up current working codebase
- [ ] Create migration branch
- [ ] Set up ESP-IDF development environment
- [ ] Install required components (LVGL, esp-idf-owb, etc.)

### Phase 1: Arduino Core APIs
- [ ] Create `platform_compat.h`
- [ ] Replace `Serial.*` with `esp_log`
- [ ] Replace `pinMode()`/`digitalWrite()` with `gpio_*`
- [ ] Replace `delay()` with `vTaskDelay()`
- [ ] Replace `millis()` with `esp_timer_get_time()`
- [ ] Replace `ESP.restart()` with `esp_restart()`
- [ ] Test basic boot and logging

### Phase 2: Storage (NVS)
- [ ] Replace `Preferences` with `nvs_*` API
- [ ] Update `Storage::begin()`
- [ ] Update `Storage::getInt()/setInt()`
- [ ] Update `Storage::getString()/setString()`
- [ ] Update `Storage::isKey()`
- [ ] Test read/write operations
- [ ] Test default value handling

### Phase 3: Rotary Encoder
- [ ] Implement rotary encoder state machine
- [ ] Create `rotary_encoder.c/h`
- [ ] Update `InputManager` to use new implementation
- [ ] Test encoder direction detection
- [ ] Test button press handling
- [ ] Verify ISR performance

### Phase 4: DS18B20 Temperature Sensor
- [ ] Add `esp-idf-owb` and `ds18b20` components
- [ ] Update `DS18B20Manager::begin()`
- [ ] Update `DS18B20Manager::update()` (state machine)
- [ ] Update temperature reading logic
- [ ] Update resolution control
- [ ] Test async polling
- [ ] Test error recovery

### Phase 5: WiFi Provisioning
- [ ] Choose provisioning method
- [ ] Implement new provisioning API
- [ ] Update `NetworkService::autoConnect()`
- [ ] Update `NetworkService::resetSettings()`
- [ ] Update `Initialization` screen
- [ ] Test provisioning flow
- [ ] Test credential persistence

### Phase 6: Display (LVGL)
- [ ] Set up LVGL with SH1106 driver
- [ ] Configure `lv_conf.h`
- [ ] Create `lv_port_disp.c` for hardware interface
- [ ] Update `DisplayManager` wrapper
- [ ] Port screen views one by one
- [ ] Port fonts and icons
- [ ] Test all UI screens
- [ ] Optimize performance

### Phase 7: Testing
- [ ] Unit test all components
- [ ] Integration test complete system
- [ ] Performance benchmarking
- [ ] Memory profiling
- [ ] Long-term stability test
- [ ] Power consumption test

### Phase 8: Documentation
- [ ] Update README
- [ ] Update build instructions
- [ ] Update hardware requirements
- [ ] Document new provisioning flow
- [ ] Create migration notes for contributors

---

## Conclusion

Migrating ProofingChamber2 from Arduino to ESP-IDF is a **significant but achievable** undertaking. The biggest challenges are:

1. **Display Migration (U8G2 → LVGL):** Most complex, affects entire UI
2. **WiFi Provisioning:** Critical for user setup, needs careful UX design

However, several factors work in your favor:
- **Partial Migration:** Already using ESP-IDF GPIO and sleep APIs
- **Good Architecture:** Interface-based design makes swapping implementations easier
- **Simple Storage:** Preferences → NVS is trivial
- **Mature Libraries:** LVGL, esp-idf-owb are production-ready

**Recommendation:** Proceed with migration in phases:
1. Start with low-risk components (Arduino Core APIs, NVS) to build confidence
2. Migrate sensor and input hardware (encoder, DS18B20)
3. Replace WiFi provisioning
4. Finally tackle the display migration

Budget **10-12 weeks** for complete migration including testing.

The end result will be a leaner, faster, more maintainable codebase with full access to ESP-IDF's advanced features. Good luck! 🚀
