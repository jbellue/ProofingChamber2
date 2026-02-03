# Web Interface

The ProofingChamber2 now includes a web interface that allows you to monitor and control the device from any browser on your local network.

## Screenshot / Appearance

The web interface features a modern, gradient purple background with white cards containing:
- **Header**: Large "🍞 Proofing Chamber" title with "Temperature Control System" subtitle
- **Current Status Card**: Displays large temperature reading and current mode with animated indicator
- **Mode Control Card**: Three prominent buttons for Heating (🔥), Cooling (❄️), and Off (⏸️)
- **Temperature Settings Card**: Input fields for adjusting heating and cooling limits
- **Footer**: Version information

The interface is fully responsive and works on desktop, tablet, and mobile devices.

## Features

The web interface provides the same functionality as the physical OLED display and rotary encoder:

1. **Live Temperature Monitoring**: Real-time temperature display updated every 2 seconds
2. **Mode Control**: Switch between Heating, Cooling, and Off modes
3. **Temperature Settings**: Adjust upper and lower temperature limits for both heating and cooling modes
4. **Visual Status**: Clear indicators showing current mode and heating/cooling state

## Access

Once your ProofingChamber is connected to WiFi:

1. The IP address will be displayed on the OLED screen after boot (for 3 seconds)
2. Note down the IP address shown on the display
3. Open a web browser on any device connected to the same network
4. Navigate to: `http://[DEVICE_IP_ADDRESS]`

For example: `http://192.168.1.100`

Alternatively, you can find the IP address by checking your router's DHCP client list.

## Usage

### Monitor Status

The **Current Status** card displays:
- Current temperature reading (updates every 2 seconds)
- Active mode (Heating, Cooling, or Off)
- Visual indicator showing the current state
- **Timing information** (when active):
  - **Proofing mode**: Shows elapsed time since proofing started
  - **Cooling mode**: Shows countdown until proofing starts and the scheduled start time

### Control Mode

Use the **Mode Control** buttons to:
- **🔥 Heating**: Start heating mode (maintains temperature between heating limits)
- **❄️ Cooling**: Start cooling mode (maintains temperature between cooling limits)  
- **⏸️ Off**: Turn off all heating/cooling

The active mode button is highlighted.

### Adjust Settings

In the **Temperature Settings** card:

1. **Heating Mode Settings**:
   - Lower Limit: Temperature at which heating turns ON
   - Upper Limit: Temperature at which heating turns OFF
   
2. **Cooling Mode Settings**:
   - Lower Limit: Temperature at which cooling turns OFF
   - Upper Limit: Temperature at which cooling turns ON

3. Click **💾 Save Settings** to apply changes

Settings are saved to non-volatile storage and persist across reboots.

## Concurrent Operation

The web interface and physical interface (OLED + rotary encoder) work together seamlessly:

- Settings changed via web are immediately available on the physical interface
- Temperature readings are shared between both interfaces
- Mode changes are synchronized between interfaces

### Important Notes

**Starting Operations:**
- Both interfaces can start heating or cooling modes independently
- Changes take effect immediately regardless of which interface initiates them

**Stopping Operations:**
- When actively running a heating or cooling cycle (screen showing temperature graph):
  - The physical button press will stop the cycle and return to menu
  - Web interface mode changes will stop heating/cooling but won't automatically exit the screen
  - To fully exit a cycle started via physical interface, use the physical button
  
**Best Practices:**
- If you start a cycle via the physical interface (menu), stop it via the physical button for proper screen navigation
- If you start a cycle via the web interface, you can stop it via web
- Settings can be changed from either interface at any time
- The web interface is best for monitoring and remote control when not actively using the physical interface

## API Endpoints

For advanced users or integration with other systems, the following REST API endpoints are available:

### GET /api/status
Returns current device status:
```json
{
  "temperature": 25.5,
  "mode": "heating",
  "isHeating": true,
  "isCooling": false,
  "proofingStartTime": 1738599600,
  "proofingElapsedSeconds": 932
}
```

When cooling with scheduled proofing:
```json
{
  "temperature": 5.2,
  "mode": "cooling",
  "isHeating": false,
  "isCooling": true,
  "coolingEndTime": 1738607700,
  "coolingRemainingSeconds": 8144
}
```

### GET /api/settings
Returns temperature settings:
```json
{
  "heating": {
    "lowerLimit": 23,
    "upperLimit": 32
  },
  "cooling": {
    "lowerLimit": 2,
    "upperLimit": 7
  }
}
```

### POST /api/mode
Change operating mode:
- Parameter: `mode` = `heating`, `cooling`, or `off`
- Returns: `{"status":"ok","mode":"heating"}`

### POST /api/settings
Update temperature settings:
- Parameters: `heating_lower`, `heating_upper`, `cooling_lower`, `cooling_upper`
- Returns: `{"status":"ok","message":"Settings updated"}`

## Troubleshooting

**Can't access the web interface:**
- Verify your device is connected to WiFi (check display during initialization)
- Ensure your computer/phone is on the same network
- Try accessing via IP address rather than hostname
- Check that port 80 is not blocked by firewall

**Settings not saving:**
- Ensure you click the "Save Settings" button
- Check that values are valid integers
- Verify storage is working (settings should persist after reboot)

**Temperature not updating:**
- Check that the DS18B20 sensor is properly connected
- Verify sensor is reading correctly on the OLED display
- Refresh the browser page

## Technical Details

- Web server runs on port 80
- Uses ESPAsyncWebServer for efficient async request handling
- Updates status every 2 seconds via AJAX
- Responsive design works on desktop and mobile devices
- All data persisted using ESP32 NVS (Preferences)
