# Hardware Testing Checklist

This checklist should be completed when testing the web interface on actual hardware.

## Pre-Testing Setup

- [ ] Ensure ESP32-C3 device is properly wired according to schematic
- [ ] DS18B20 temperature sensor is connected and functional
- [ ] Device has been flashed with the new firmware
- [ ] Device is connected to WiFi network
- [ ] Note the device's IP address (check OLED display or router)

## Basic Connectivity

- [ ] Access web interface at `http://[device-ip]`
- [ ] Web page loads completely
- [ ] All UI elements render correctly
- [ ] No JavaScript errors in browser console
- [ ] Page is responsive (test on desktop and mobile)

## Status Monitoring

- [ ] Current temperature displays and matches OLED reading
- [ ] Temperature updates automatically (every ~2 seconds)
- [ ] Mode indicator shows correct state (off/heating/cooling)
- [ ] Mode indicator animation works (pulsing effect)
- [ ] Active mode button is highlighted correctly

## Mode Control - Heating

- [ ] Click "Heating" button
- [ ] Success message appears
- [ ] Mode changes to "heating"
- [ ] Heating indicator pulses red
- [ ] Physical LED (GPIO 6) turns on
- [ ] Temperature controller activates heating relay when temp < lower limit
- [ ] OLED display reflects heating mode if on ProofingController screen

## Mode Control - Cooling

- [ ] Click "Cooling" button
- [ ] Success message appears
- [ ] Mode changes to "cooling"
- [ ] Cooling indicator pulses blue
- [ ] Physical LED (GPIO 5) turns on
- [ ] Temperature controller activates cooling relay when temp > upper limit
- [ ] OLED display reflects cooling mode if on CoolingController screen

## Mode Control - Off

- [ ] Click "Off" button
- [ ] Success message appears
- [ ] Mode changes to "off"
- [ ] Both indicators turn gray
- [ ] Both physical LEDs turn off
- [ ] Both relays turn off
- [ ] Temperature is still being read

## Settings - Heating

- [ ] Current heating limits load correctly
- [ ] Change heating lower limit (e.g., to 25°C)
- [ ] Change heating upper limit (e.g., to 30°C)
- [ ] Click "Save Settings"
- [ ] Success message appears
- [ ] Refresh page - settings persist
- [ ] Reboot device - settings persist
- [ ] If in heating mode, relays respond to new limits

## Settings - Cooling

- [ ] Current cooling limits load correctly
- [ ] Change cooling lower limit (e.g., to 3°C)
- [ ] Change cooling upper limit (e.g., to 8°C)
- [ ] Click "Save Settings"
- [ ] Success message appears
- [ ] Refresh page - settings persist
- [ ] Reboot device - settings persist
- [ ] If in cooling mode, relays respond to new limits

## Input Validation

- [ ] Try entering temperature < -50 (should be prevented by browser)
- [ ] Try entering temperature > 100 (should be prevented by browser)
- [ ] Try entering non-numeric value (should be prevented by browser)
- [ ] Verify server still validates on backend (manual API test optional)

## Concurrent Operation - Web to Physical

- [ ] Start heating via web interface
- [ ] Press physical button
- [ ] Heating stops
- [ ] Screen returns to menu
- [ ] Web interface still shows status correctly

## Concurrent Operation - Physical to Web

- [ ] Use physical menu to start heating
- [ ] Open web interface
- [ ] Verify web shows "heating" mode
- [ ] Verify temperature updates
- [ ] Change mode to "off" via web
- [ ] Heating stops (relay/LED off)
- [ ] Physical screen still shows ProofingController (expected behavior)
- [ ] Press physical button to return to menu

## Concurrent Operation - Settings

- [ ] View current settings on OLED (via menu)
- [ ] Change settings via web interface
- [ ] Save settings
- [ ] Check OLED menu - settings should reflect changes immediately
- [ ] Start a heating cycle
- [ ] Verify new limits are in effect

## Long-term Stability

- [ ] Leave web page open for 10+ minutes
- [ ] Verify updates continue every 2 seconds
- [ ] Check browser console for errors or memory leaks
- [ ] Verify device doesn't crash or hang
- [ ] Check device memory usage (serial monitor if debug enabled)

## Multi-client

- [ ] Open web interface on two different devices simultaneously
- [ ] Change mode from device 1
- [ ] Verify device 2 updates within 2 seconds
- [ ] Change settings from device 2
- [ ] Verify device 1 sees updated settings

## Edge Cases

- [ ] Disconnect WiFi temporarily
- [ ] Reconnect WiFi
- [ ] Verify web interface becomes accessible again
- [ ] Verify device continued operating correctly during WiFi outage
- [ ] Test API endpoints directly with curl (optional):
  ```bash
  curl http://[device-ip]/api/status
  curl http://[device-ip]/api/settings
  curl -X POST -d "mode=heating" http://[device-ip]/api/mode
  curl -X POST -d "heating_lower=25&heating_upper=30" http://[device-ip]/api/settings
  ```

## Performance

- [ ] Initial page load time < 2 seconds
- [ ] Status updates are smooth (no jank)
- [ ] Mode changes are nearly instant
- [ ] Settings save quickly
- [ ] Device remains responsive to physical controls during web activity

## Known Limitations (Verify Expected Behavior)

- [ ] When in ProofingController or CoolingController screen:
  - Web mode change stops heating/cooling
  - But screen doesn't auto-exit (need physical button)
  - This is expected behavior (documented)

## Security (Local Network)

- [ ] Verify web interface is NOT accessible from outside local network
- [ ] Confirm no authentication is required (expected for local device)
- [ ] Check that input validation prevents invalid values
- [ ] Verify no sensitive data is exposed in API responses

## Documentation Verification

- [ ] WEB_INTERFACE.md instructions are accurate
- [ ] All documented features work as described
- [ ] Any discrepancies noted and fixed

## Completion

Date tested: ___________
Tested by: ___________
Hardware version: ___________
Firmware commit: 972e662

Issues found:
- 
- 
- 

Pass/Fail: ___________
