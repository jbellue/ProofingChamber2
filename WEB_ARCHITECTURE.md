# Web Interface Architecture - Menu Actions Integration

## Problem Statement

The initial web interface implementation had a fundamental issue:
- Web requests directly called `tempController->setMode()` to change modes
- This bypassed the normal menu navigation system
- Physical OLED display stayed on whatever screen it was on
- **Result:** Physical display out of sync with actual device state

**Example of the problem:**
1. User navigates physical menu to settings
2. User clicks "Heating" on web interface
3. Heating starts, but physical display still shows settings menu
4. User confused - display doesn't reflect what's happening

## Solution: Use Menu Actions

Instead of directly manipulating the temperature controller, web requests now trigger the same menu actions that physical buttons use.

### Architecture Flow

**Before (Direct Mode Change):**
```
Web Request → tempController->setMode() → Relay Changes
                                        ↓
                                   (Screen unchanged)
```

**After (Menu Action):**
```
Web Request → menuActions->proofNowAction() → Screen Transition
                                            ↓
                                     ProofingController
                                            ↓
                                   tempController->setMode()
                                            ↓
                                      Relay Changes
```

### Implementation Details

#### 1. Added Menu System to AppContext

**AppContext.h changes:**
```cpp
// Added forward declarations
class MenuActions;
class Menu;

struct AppContext {
    // ... existing members ...
    
    // Menu and actions
    MenuActions* menuActions = nullptr;
    Menu* menu = nullptr;
};
```

#### 2. Updated WebServerService

**New includes:**
```cpp
#include "../MenuActions.h"
#include "../screens/Menu.h"
#include "../ScreensManager.h"
```

**Refactored handleSetMode():**

**Heating Mode:**
```cpp
if (mode == "heating") {
    _ctx->menuActions->proofNowAction();
    // This properly transitions to ProofingController
    // Physical screen now shows proofing display
}
```

**Off Mode:**
```cpp
if (mode == "off") {
    // Turn off temperature control
    if (_ctx->tempController) {
        _ctx->tempController->setMode(ITemperatureController::OFF);
    }
    // Return to menu screen
    _ctx->screens->setActiveScreen(_ctx->menu);
    // Physical screen now shows menu
}
```

**Cooling Mode:**
```cpp
if (mode == "cooling") {
    // Returns informative error
    // Cooling requires time configuration (proof-in-X or proof-at-Y)
    // Can't be directly triggered without time params
}
```

#### 3. Screen Synchronization

The menu action `proofNowAction()` does everything properly:

```cpp
void MenuActions::proofNowAction() {
    BaseController* menu = _ctx->screens->getActiveScreen();
    menu->setNextScreen(_proofingController);
    _proofingController->setNextScreen(menu);
}
```

When ScreensManager processes this:
1. Current screen's `begin()` is called on ProofingController
2. ProofingController initializes (sets mode, starts timer, etc.)
3. Physical display draws proofing screen
4. Everything is in sync!

### Behavior Changes

| Action | Old Behavior | New Behavior |
|--------|-------------|--------------|
| Web: Start Heating | Mode changes, screen unchanged | Transitions to proofing screen ✅ |
| Web: Stop | Mode changes, screen unchanged | Returns to menu ✅ |
| Physical: Start | Transitions to proofing screen | Same (unchanged) |
| Physical: Stop | Returns to menu | Same (unchanged) |

### Code Reuse

This approach maximizes code reuse:
- ✅ Uses existing, tested menu actions
- ✅ Same code paths as physical interface
- ✅ Consistent behavior
- ✅ Proper lifecycle management
- ✅ No duplicate logic

### Edge Cases Handled

**Already on Menu:**
```cpp
if (currentScreen != _ctx->menu) {
    // transition to menu
} else {
    // already at menu, acknowledge request
}
```

**Menu System Not Ready:**
```cpp
if (!_ctx->menuActions || !_ctx->screens || !_ctx->menu) {
    return error;
}
```

## Benefits

### User Experience
1. **Physical display always correct** - shows what's actually happening
2. **Consistent behavior** - web and physical work the same way
3. **No confusion** - what you see matches reality
4. **Proper feedback** - screen transitions provide visual confirmation

### Code Quality
1. **DRY principle** - Don't Repeat Yourself
2. **Single source of truth** - menu actions handle navigation
3. **Testable** - existing menu action tests still apply
4. **Maintainable** - changes to navigation logic in one place

### Technical
1. **Thread-safe** - ESP32 is single-threaded cooperative
2. **No race conditions** - sequential processing
3. **Proper lifecycle** - ScreensManager handles begin/end
4. **Memory efficient** - no additional state tracking needed

## Limitations

### Cooling Mode Not Supported via Web

**Why:**
Cooling mode requires time configuration:
- "Proof in X hours/minutes" (relative time)
- "Proof at Y time" (absolute time)

**Current workaround:**
- User can start cooling via physical interface
- Web can monitor status (countdown timer shown)
- Web can stop cooling

**Future enhancement:**
Add dedicated cooling endpoint:
```
POST /api/cooling/schedule
{
    "type": "relative", // or "absolute"
    "hours": 2,
    "minutes": 30
}
```

This would call the appropriate menu actions with time configuration.

## Testing Recommendations

### Manual Testing

1. **Test Heating Start:**
   - Navigate to some screen on physical interface (e.g., settings)
   - Click "Heating" on web
   - Verify physical display transitions to proofing screen
   - Verify temperature control starts
   - Verify elapsed timer shows on both web and physical display

2. **Test Off from Web:**
   - Start proofing via web
   - Verify proofing screen visible
   - Click "Off" on web
   - Verify physical display returns to menu
   - Verify temperature control stops

3. **Test Physical Button During Web-Started Proofing:**
   - Start proofing via web
   - Press physical button
   - Verify returns to menu
   - Verify web interface updates (within 2 seconds)

4. **Test Settings Persistence:**
   - Change settings via web
   - Navigate to settings on physical interface
   - Verify settings match

### Automated Testing (Future)

```cpp
// Mock test for menu action integration
TEST(WebServer, HeatingModeTriggersProofingAction) {
    MockMenuActions menuActions;
    EXPECT_CALL(menuActions, proofNowAction()).Times(1);
    
    webServer.handleSetMode("heating");
    
    // Verify menu action was called
}
```

## Conclusion

This refactoring fundamentally improves the web interface by:
- Treating web requests as "virtual button presses"
- Using the existing, proven menu navigation system
- Ensuring physical display always reflects actual state
- Maintaining code quality and reusability

The result is a web interface that "plays nice" with the physical interface, providing a seamless experience regardless of which interface the user chooses to use.
