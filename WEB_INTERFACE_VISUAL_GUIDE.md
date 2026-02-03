# Web Interface Visual Guide

## What You See in the Web Browser

```
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│  🍞 Proofing Chamber                                        │
│     Temperature Control System                              │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Current Status                                             │
│  ┌────────────────┬──────────────────┬──────────────────┐  │
│  │  Temperature   │  Current Screen  │      Mode        │  │
│  │    25.5°C     │      Menu        │   Off ⚫          │  │
│  └────────────────┴──────────────────┴──────────────────┘  │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Virtual Controls                                           │
│  These buttons simulate physical interface interactions     │
│                                                              │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐                 │
│  │  ⬆️ Up   │  │ ⬇️ Down  │  │ ✓ Select │                 │
│  └──────────┘  └──────────┘  └──────────┘                 │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Quick Actions (Legacy)                                     │
│  Note: These may not work correctly. Use Virtual Controls  │
│                                                              │
│  ┌──────────────────┐  ┌──────────────────┐               │
│  │ 🔥 Start Heating │  │    ⏸️ Stop      │               │
│  └──────────────────┘  └──────────────────┘               │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Temperature Settings                                       │
│  ... (existing settings interface) ...                     │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

## Usage Scenarios

### Scenario 1: Starting Proofing from Web

**Initial State:**
```
Current Status
┌────────────────┬──────────────────┬──────────────────┐
│  Temperature   │  Current Screen  │      Mode        │
│    22.0°C     │      Menu        │   Off ⚫          │
└────────────────┴──────────────────┴──────────────────┘
```

**User Actions:**
1. Click "Down" 3 times to scroll menu
2. Click "Select" to choose "Mettre en pousse"

**Result After ~2 Seconds:**
```
Current Status
┌────────────────┬──────────────────┬──────────────────┐
│  Temperature   │  Current Screen  │      Mode        │
│    22.2°C     │    Proofing      │ Heating 🔴       │
└────────────────┴──────────────────┴──────────────────┘

⏱️ Proofing: 5s elapsed
```

**What the Physical Display Shows:**
```
┌────────────────┐
│  Mettre en     │
│    pousse      │
│                │
│   25.5°C       │
│   [Graph]      │
│   00:00:05     │
└────────────────┘
```

**Both match!** ✅

---

### Scenario 2: Navigating Settings

**User Actions:**
1. Click "Down" to scroll to "Réglages"
2. Click "Select" to enter settings
3. Click "Down" to "Chaud"
4. Click "Select" to enter hot settings

**Web Shows:**
```
Current Screen: Menu
↓
Current Screen: Menu
↓  
Current Screen: Menu
↓
Current Screen: Menu
```

**Physical Display Shows:**
```
Settings Menu
> Réglages chaud
  Réglages froid
  ...
```

**Web mirrors the navigation!** User knows where they are.

---

### Scenario 3: Stopping Active Proofing

**Initial State:**
```
Current Status
┌────────────────┬──────────────────┬──────────────────┐
│  Temperature   │  Current Screen  │      Mode        │
│    28.5°C     │    Proofing      │ Heating 🔴       │
└────────────────┴──────────────────┴──────────────────┘

⏱️ Proofing: 15m 30s elapsed
```

**User Action:**
Click "Select" button

**Result:**
```
Current Status
┌────────────────┬──────────────────┬──────────────────┐
│  Temperature   │  Current Screen  │      Mode        │
│    28.5°C     │      Menu        │   Off ⚫          │
└────────────────┴──────────────────┴──────────────────┘
```

**Physical Display:**
```
┌────────────────┐
│ Menu Principal │
│  > Mettre en   │
│    pousse      │
│    Pousser     │
│    dans...     │
└────────────────┘
```

**Back to menu on both!** ✅

---

## Button Functions

### Virtual Controls

**⬆️ Up Button:**
- Sends: `POST /api/input/encoder?steps=-1`
- Effect: Counter-clockwise encoder turn
- In Menu: Scrolls up
- In Value Edit: Decreases value

**⬇️ Down Button:**
- Sends: `POST /api/input/encoder?steps=1`
- Effect: Clockwise encoder turn
- In Menu: Scrolls down
- In Value Edit: Increases value

**✓ Select Button:**
- Sends: `POST /api/input/button`
- Effect: Button press
- In Menu: Selects item, enters submenu
- During Operation: Cancels, returns to menu
- In Edit: Confirms value

### Legacy Quick Actions

**🔥 Start Heating:**
- Old approach: `POST /api/mode?mode=heating`
- May not work reliably
- Use Virtual Controls instead

**⏸️ Stop:**
- Old approach: `POST /api/mode?mode=off`
- May not work reliably
- Use Virtual Controls instead

---

## Status Display Elements

### Temperature
- Shows current temperature from DS18B20 sensor
- Updates every 2 seconds
- Example: `25.5°C`

### Current Screen
- **NEW!** Shows active screen name
- Updates every 2 seconds
- Possible values:
  - `Menu` - Main menu or submenus
  - `Proofing` - Active proofing with timer
  - `Cooling` - Active cooling with countdown
  - `Initialization` - During startup
  - `Unknown` - Fallback

### Mode
- Shows temperature control mode
- Values:
  - `Off ⚫` - No heating or cooling
  - `Heating 🔴` - Heater active (pulsing animation)
  - `Cooling 🔵` - Cooler active (pulsing animation)

### Timing Info (When Active)
- **During Proofing:**
  - `⏱️ Proofing: 15m 30s elapsed`
  
- **During Cooling:**
  - `🕐 Cooling: 2h 15m until proofing starts`
  - `Starts at: 2/3/2026, 6:00:00 PM`

---

## Interaction Flow

```
┌─────────────────────────────────────────────────────────┐
│                                                         │
│  1. User opens web browser                              │
│     → Sees "Current Screen: Menu"                       │
│                                                         │
│  2. User clicks "Down" 3 times                          │
│     → Each click: POST /api/input/encoder?steps=1       │
│     → Menu scrolls on physical display                  │
│     → Web still shows "Current Screen: Menu"            │
│                                                         │
│  3. User clicks "Select"                                │
│     → POST /api/input/button                            │
│     → Menu handles selection                            │
│     → Transitions to ProofingController                 │
│     → Physical display shows proofing screen            │
│                                                         │
│  4. Web polls (2 seconds later)                         │
│     → GET /api/display/state                            │
│     → Returns: {"screen":"Proofing"}                    │
│     → Web shows "Current Screen: Proofing"              │
│                                                         │
│  5. User clicks "Select" to stop                        │
│     → POST /api/input/button                            │
│     → ProofingController exits                          │
│     → Returns to Menu                                   │
│     → Physical display shows menu                       │
│                                                         │
│  6. Web polls (2 seconds later)                         │
│     → GET /api/display/state                            │
│     → Returns: {"screen":"Menu"}                        │
│     → Web shows "Current Screen: Menu"                  │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

---

## Technical Details

### Polling Frequency
- Status updates: Every 2 seconds
- Display state: Every 2 seconds (same request)
- No WebSocket (could be added later)

### Input Processing
- Virtual inputs queued immediately
- Processed in next update loop iteration
- Typically < 100ms latency
- Web refreshes within 2 seconds

### Screen Synchronization
- Web shows what device shows
- 2 second max delay for web to update
- Device state always authoritative
- No way to get out of sync

---

## Troubleshooting

### Web Shows Wrong Screen
**Symptom:** Web says "Menu" but device shows "Proofing"
**Cause:** Polling delay (max 2 seconds)
**Solution:** Wait a moment, will auto-update

### Buttons Don't Work
**Symptom:** Clicking buttons does nothing
**Possible Causes:**
1. Device not connected to WiFi → Check IP address
2. Wrong IP address → Verify from device display
3. Network issue → Check connection

### Display Goes Black
**This should not happen with virtual controls!**
- If it does, report as bug
- Old quick action buttons might still have this issue
- Use virtual controls instead

---

## Comparison: Old vs New

### Old Approach (Broken)
```
Web Browser
     ↓
 [Start Heating]
     ↓
POST /api/mode?mode=heating
     ↓
tempController->setMode(HEATING)
     ↓
Heating starts ✓
Physical display unchanged ✗
```

### New Approach (Working)
```
Web Browser
     ↓
  [⬇️ Down] [✓ Select]
     ↓
POST /api/input/encoder
POST /api/input/button
     ↓
InputManager queues inputs
     ↓
Update loop processes
     ↓
Menu handles inputs
     ↓
Transitions to ProofingController ✓
Physical display updates ✓
     ↑
     │
  GET /api/display/state
     │
Web updates ✓
```

---

## Summary

The new web interface is a **true display mirror with virtual remote control**:

✅ Shows what the physical display shows ("Current Screen")
✅ Sends virtual inputs (Up/Down/Select buttons)
✅ Device processes inputs naturally
✅ Everything stays synchronized
✅ Simple and robust

Use the **Virtual Controls** for reliable operation. The Legacy Quick Actions are kept for convenience but may not work correctly in all situations.
