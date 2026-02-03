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

The web interface and physical interface (OLED + rotary encoder) now work together seamlessly with **full synchronization**:

- Settings changed via web are immediately available on the physical interface
- Temperature readings are shared between both interfaces
- **Mode changes trigger proper screen transitions** - physical display stays in sync!

### How It Works

**Starting Proofing from Web:**
1. Click "🔥 Heating" button on web interface
2. Device transitions to proofing screen (same as pressing button on menu)
3. Physical OLED shows proofing screen with temperature and timer
4. Both interfaces show synchronized state ✅

**Stopping from Web:**
1. Click "⏸️ Off" button on web interface
2. Device returns to menu screen
3. Physical OLED shows menu
4. Temperature control stops
5. Everything stays in sync ✅

**Stopping from Physical Button:**
1. Press physical button while proofing/cooling
2. Returns to menu
3. Web interface reflects the change on next update (within 2 seconds)

### Important Notes

**Heating Mode:**
- ✅ Fully supported via web interface
- ✅ Triggers proper screen transition to proofing screen
- ✅ Physical display shows proofing progress
- ✅ Can be stopped from either web or physical button

**Cooling Mode:**
- ⚠️ Not directly available via web (requires time configuration)
- ✅ Can be started via physical interface
- ✅ Web interface can monitor cooling status and timing
- ✅ Web interface can stop cooling operation
- Future enhancement: Add dedicated cooling schedule endpoint

**Off Mode:**
- ✅ Fully supported via web interface
- ✅ Returns device to menu screen
- ✅ Physical display shows menu
- ✅ Stops any active operation

### Best Practices
- Use web interface to start/stop heating and monitor remotely
- Use physical interface for cooling with scheduled proofing (requires time setup)
- Both interfaces always show consistent state
- No more screen desynchronization issues!

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
