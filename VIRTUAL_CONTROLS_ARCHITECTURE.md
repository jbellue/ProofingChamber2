# Virtual Controls Architecture

## Problem Statement

The previous menu actions approach had fundamental issues:
1. Web starting proofing → display stayed on menu (didn't transition)
2. Stopping from web → display went black
3. Async web requests conflicted with synchronous screen lifecycle

**Root cause:** Menu actions work within the normal update loop. Web requests are asynchronous and can't properly trigger screen transitions.

## Solution: Virtual Input Injection

Instead of trying to control screens directly, make the web interface simulate physical inputs. The device processes these exactly like real button presses and encoder turns.

### Architecture

```
┌─────────────────┐
│   Web Browser   │
│                 │
│  [Up] [Down]   │
│    [Select]     │
└────────┬────────┘
         │
         │ POST /api/input/encoder?steps=1
         │ POST /api/input/button
         ↓
┌─────────────────────────────┐
│    WebServerService         │
│  - handleInjectEncoder()    │
│  - handleInjectButton()     │
└────────┬────────────────────┘
         │
         │ injectEncoderSteps(1)
         │ injectButtonPress()
         ↓
┌─────────────────────────────┐
│      InputManager           │
│  _pendingSteps += steps     │
│  _buttonPressed = true      │
└────────┬────────────────────┘
         │
         │ Normal update loop
         ↓
┌─────────────────────────────┐
│    ScreensManager           │
│  activeScreen->update()     │
│    - Menu processes steps   │
│    - Scrolls, selects, etc. │
└────────┬────────────────────┘
         │
         │ Screen transitions happen naturally
         ↓
┌─────────────────────────────┐
│   Physical Display          │
│  Shows current screen       │
│  (Menu, Proofing, etc.)     │
└─────────────────────────────┘
         ↑
         │
         │ GET /api/display/state
         │
┌─────────────────┐
│   Web Browser   │
│ "Current Screen:│
│    Proofing"    │
└─────────────────┘
```

## Implementation Details

### 1. Virtual Input Methods

**InputManager.h/cpp:**
```cpp
class IInputManager {
    // New methods
    virtual void injectButtonPress() = 0;
    virtual void injectEncoderSteps(int steps) = 0;
};

class InputManager : public IInputManager {
    void injectButtonPress() override {
        _buttonPressed = true;  // Same flag as physical button
    }
    
    void injectEncoderSteps(int steps) override {
        _pendingSteps += steps;  // Same accumulator as physical encoder
    }
};
```

**Key insight:** These set the SAME internal state as physical inputs. No special handling needed.

### 2. Display State Exposure

**BaseController.h:**
```cpp
class BaseController {
    // New method
    virtual const char* getScreenName() const { return "Unknown"; }
};
```

Each screen overrides this:
- Menu: "Menu"
- ProofingController: "Proofing"
- CoolingController: "Cooling"
- etc.

### 3. Web API Endpoints

**POST /api/input/button**
- Injects button press
- Returns: `{"status":"ok","action":"button_press"}`

**POST /api/input/encoder?steps=N**
- Injects encoder steps
- Positive = clockwise (down in menu)
- Negative = counter-clockwise (up in menu)
- Range: -10 to 10 for safety
- Returns: `{"status":"ok","action":"encoder_turn","steps":1}`

**GET /api/display/state**
- Returns current screen name
- Returns: `{"screen":"Menu","temperature":25.5,"mode":"off"}`

### 4. Web UI

**Display Elements:**
```html
Current Screen: <span id="currentScreen">Menu</span>
```

**Virtual Controls:**
```html
<button onclick="encoderUp()">⬆️ Up</button>
<button onclick="encoderDown()">⬇️ Down</button>
<button onclick="pressButton()">✓ Select</button>
```

**JavaScript:**
```javascript
async function encoderUp() {
    const formData = new FormData();
    formData.append('steps', '-1');  // Negative = up
    await fetch('/api/input/encoder', { method: 'POST', body: formData });
    setTimeout(updateStatus, 100);  // Quick refresh
}

async function updateStatus() {
    const displayResponse = await fetch('/api/display/state');
    const data = await displayResponse.json();
    document.getElementById('currentScreen').textContent = data.screen;
}
```

## Benefits

### 1. Perfect Synchronization
- Web inputs → processed in normal loop → screen updates naturally
- Physical display always shows correct state
- No race conditions or timing issues

### 2. Code Reuse
- Zero changes to existing screen logic
- Menu, ProofingController, etc. work unchanged
- All existing behavior preserved

### 3. Simplicity
- No special web-only code paths
- No async screen transitions
- No lifecycle management issues

### 4. True Remote Control
- Web acts like a physical remote control
- Can navigate menus just like physical interface
- Can start/stop operations properly

### 5. Robustness
- Inputs queued and processed sequentially
- Can't break screen state machine
- Natural debouncing via update loop

## Usage Examples

### Example 1: Start Proofing from Web

**User actions:**
1. See "Current Screen: Menu"
2. Click Down several times to scroll to "Mettre en pousse"
3. Click Select
4. See "Current Screen: Proofing"
5. Physical display shows proofing screen with timer

**What happens internally:**
```
Click Down → inject steps → menu scrolls → no transition yet
Click Down → inject steps → menu scrolls → no transition yet  
Click Select → inject button → menu handles selection → transitions to ProofingController
ProofingController.begin() → sets mode, initializes display
Next updateStatus() → sees screen="Proofing"
```

### Example 2: Stop Proofing from Web

**User actions:**
1. See "Current Screen: Proofing"
2. Click Select (acts as cancel button)
3. See "Current Screen: Menu"
4. Physical display shows menu

**What happens internally:**
```
Click Select → inject button → ProofingController.update() sees button press
ProofingController exits → returns false → ScreensManager transitions to next (Menu)
Menu.begin() called → display shows menu
Next updateStatus() → sees screen="Menu"
```

### Example 3: Adjust Settings from Web

**User actions:**
1. Navigate to Settings menu
2. Navigate to Hot submenu
3. Navigate to Lower Limit
4. Click Select to edit
5. Click Up/Down to adjust value
6. Click Select to confirm

**All processed exactly like physical interface!**

## Comparison with Previous Approaches

### Approach 1: Direct Mode Changes (Original)
```cpp
// Web request directly sets mode
tempController->setMode(HEATING);
// ❌ Screen doesn't transition
// ❌ Display out of sync
```

### Approach 2: Menu Actions (Previous Fix)
```cpp
// Web request calls menu action
menuActions->proofNowAction();
// ❌ Async request vs sync lifecycle
// ❌ Screen transition timing issues
// ❌ Display goes black on stop
```

### Approach 3: Virtual Inputs (Current)
```cpp
// Web request injects input
inputManager->injectButtonPress();
// ✅ Processed in normal loop
// ✅ Screen transitions naturally
// ✅ Display always in sync
```

## Edge Cases Handled

### Multiple Rapid Clicks
- Inputs queued in _pendingSteps and _buttonPressed
- Processed one per update loop iteration
- Natural rate limiting

### Web and Physical Simultaneously
- Both add to same counters
- Processed together in update loop
- No conflicts

### Network Latency
- Web sees slightly delayed state (2 second polling)
- But actual device state always correct
- User can keep clicking, inputs queue up

### Screen Transitions
- Inputs processed by current screen
- If screen changes, next input goes to new screen
- Natural and intuitive behavior

## Testing Recommendations

### Unit Tests (Future)
```cpp
TEST(InputManager, InjectButtonPress) {
    InputManager input(...);
    input.injectButtonPress();
    EXPECT_TRUE(input.isButtonPressed());
    EXPECT_FALSE(input.isButtonPressed());  // Consumed
}

TEST(InputManager, InjectEncoderSteps) {
    InputManager input(...);
    input.injectEncoderSteps(3);
    EXPECT_EQ(input.getPendingSteps(), 3);
}
```

### Integration Tests (Hardware)

**Test 1: Menu Navigation**
1. Device on menu
2. Web shows "Current Screen: Menu"
3. Click Down 3 times
4. Physical display scrolls down
5. Web shows same menu position

**Test 2: Start Proofing**
1. Navigate to proof option via web
2. Click Select
3. Physical display transitions to proofing
4. Web shows "Current Screen: Proofing"
5. Both show same timer

**Test 3: Stop from Web**
1. Proofing active
2. Click Select from web
3. Physical display returns to menu
4. Web shows "Current Screen: Menu"

**Test 4: Settings Adjustment**
1. Navigate to settings via web
2. Adjust value via Up/Down buttons
3. Click Select to confirm
4. Physical display shows updated value
5. Web can read updated value

## Limitations

### 1. No Menu Content Display
- Web shows screen name only
- Doesn't show menu items or current selection
- Could be added in future by exposing menu state

### 2. Polling Delay
- Web polls every 2 seconds
- Slight delay before web shows screen change
- Doesn't affect actual operation (inputs processed immediately)

### 3. No Visual Feedback on Physical Display
- Virtual inputs don't show differently
- Can't tell if input came from web or physical
- This is actually a feature (transparent operation)

## Future Enhancements

### 1. Menu State Exposure
```cpp
// In Menu.h
struct MenuState {
    const char* menuName;
    MenuItem* items;
    int currentIndex;
    int itemCount;
};

MenuState getMenuState() const;
```

Web could display actual menu items and highlight selection.

### 2. WebSocket for Real-time Updates
```javascript
const ws = new WebSocket('ws://device/api/display');
ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    updateDisplay(data.screen);
};
```

Eliminate polling delay.

### 3. Keyboard Shortcuts
```javascript
document.addEventListener('keydown', (e) => {
    if (e.key === 'ArrowUp') encoderUp();
    if (e.key === 'ArrowDown') encoderDown();
    if (e.key === 'Enter') pressButton();
});
```

Use keyboard as remote control.

### 4. Touch Gestures (Mobile)
```javascript
// Swipe up/down for encoder
// Tap for button press
```

Better mobile experience.

## Conclusion

The virtual input approach is:
- ✅ Simple to implement
- ✅ Robust and reliable
- ✅ Guarantees synchronization
- ✅ Reuses all existing code
- ✅ Easy to understand and debug

By treating the web interface as a "virtual remote control" that sends inputs rather than commands, we achieve perfect synchronization with the physical display while keeping the architecture simple and maintainable.
