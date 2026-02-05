# Architecture Plan: Display and Web Synchronization

## Current State Analysis

### Architecture Components

1. **Model (State):**
   - ProofingController: Manages proofing state (_startTime, heating mode)
   - CoolingController: Manages cooling state (_endTime, cooling mode)
   - TemperatureController: Manages heating/cooling relays

2. **Views (Display):**
   - Physical OLED: ProofingView, CoolingView
   - Web Interface: HTML + WebSocket

3. **Controllers (Input):**
   - Physical: Button handlers via InputManager
   - Web: REST API endpoints in WebServerService

### Identified Problems

1. **Display Update Issues:**
   - When action triggered from web, display calls begin() but may not fully refresh
   - View state not properly initialized
   - Display buffer may not be sent to OLED

2. **Web Update Issues:**
   - WebSocket sends updates every 500ms (polling)
   - Also sends on state change via notifyStateChange()
   - But web doesn't show proper state in all cases

3. **Synchronization Issues:**
   - No single source of truth
   - State spread across multiple components
   - Views don't consistently update when state changes

4. **Action Validation:**
   - No validation before executing actions
   - Can start proof from web while in settings menu
   - Web buttons always enabled

## Proposed Solution

### Core Principles

1. **Single Source of Truth:** Controllers own state
2. **Event-Driven Updates:** State changes trigger view updates
3. **Validation Before Action:** Check if action is valid before executing
4. **Consistent View Updates:** All views update the same way

### Architecture Changes

#### 1. State Management (No Changes Needed)
- Controllers already manage state correctly
- ProofingController: _startTime, heating mode
- CoolingController: _endTime, cooling mode

#### 2. View Update Mechanism (Needs Improvement)

**Physical Display:**
- Already works via update() calls in main loop
- Problem: When web triggers action, view needs full refresh
- Solution: Ensure clearBuffer() + update(true) + sendBuffer() after navigation

**Web Display:**
- Currently: Polling every 500ms + notifyStateChange() on events
- Problem: WebSocket data format doesn't match view needs
- Solution: Send complete state snapshot on every change

#### 3. Action Validation (New Feature)

**Add validation layer:**
```cpp
bool canStartProofing() {
    // Can only start if not already active and on menu/main screen
    return !proofingController->isActive() && 
           !coolingController->isActive() &&
           screens->getCurrentScreen()->allowsActions();
}
```

**Web UI:**
- Disable buttons when action not available
- Show reason why disabled (tooltip/message)

#### 4. Input Handling (Standardize)

**Both inputs should:**
1. Validate action is allowed
2. Update model (controller state)
3. Trigger view updates (both physical and web)
4. Return success/failure

### Implementation Plan

#### Phase 1: Fix Display Refresh (Current Issue)
- [x] Ensure display fully refreshes when action from web
- [x] Call begin() followed by explicit buffer refresh
- Status: Partially done, may need refinement

#### Phase 2: Improve Web Updates
- [ ] Enhance WebSocket state format
- [ ] Include all relevant state information
- [ ] Send full state on every change (not just temperature)
- [ ] Update web UI to show complete state

#### Phase 3: Add Action Validation
- [ ] Add validation methods to controllers
- [ ] Check screen state before allowing actions
- [ ] Return validation errors to web
- [ ] Disable web buttons when actions not available

#### Phase 4: Standardize Input Handling
- [ ] Create common action execution path
- [ ] Both physical and web use same path
- [ ] Consistent state updates
- [ ] Consistent view notifications

#### Phase 5: Testing and Refinement
- [ ] Test all action combinations
- [ ] Verify display always shows correct state
- [ ] Verify web always shows correct state
- [ ] Test rapid state changes
- [ ] Test error conditions

## Implementation Details

### Phase 1: Display Refresh (Immediate)

**Issue:** Display shows blank or partial content when action from web

**Solution:**
```cpp
void ProofingController::startProofing() {
    // Set state
    _startTime = time(nullptr);
    ctx->tempController->setMode(ITemperatureController::HEATING);
    
    // Navigate to screen
    ctx->screens->setActiveScreen(this);
    begin();
    
    // Force full display refresh
    ctx->display->clearBuffer();
    update(true); // Force redraw
    ctx->display->sendBuffer();
    
    // Notify web
    if (ctx->webServerService) {
        ctx->webServerService->notifyStateChange();
    }
}
```

**Status:** Already implemented, but may need verification

### Phase 2: Web Updates (High Priority)

**Current WebSocket message:**
```json
{
    "screen": "Proofing",
    "temperature": 25.5,
    "mode": "heating",
    "isActive": true,
    "elapsedSeconds": 450
}
```

**Enhanced message (needed):**
```json
{
    "screen": "Proofing",
    "canStartProofing": false,
    "canStartCooling": false,
    "canStop": true,
    "proofing": {
        "active": true,
        "startTime": 1738684800,
        "elapsedSeconds": 450,
        "temperature": 25.5
    },
    "cooling": {
        "active": false
    },
    "mode": "heating"
}
```

### Phase 3: Action Validation (Medium Priority)

**Add to controllers:**
```cpp
class ProofingController {
public:
    bool canStart() const {
        // Not already running
        if (isActive()) return false;
        
        // Cooling not running
        if (ctx->coolingController->isActive()) return false;
        
        // On a screen that allows actions
        auto currentScreen = ctx->screens->getCurrentScreen();
        return currentScreen->allowsActions();
    }
    
    bool isActive() const {
        return _startTime > 0;
    }
};
```

**Add to Screen base class:**
```cpp
class BaseController {
public:
    virtual bool allowsActions() const { return false; }
};

// In Menu, Proofing, Cooling:
bool allowsActions() const override { return true; }
```

### Phase 4: Web UI Improvements

**Update button state:**
```javascript
function updateButtons(state) {
    // Enable/disable based on what's allowed
    document.getElementById('proof-now').disabled = !state.canStartProofing;
    document.getElementById('stop-btn').disabled = !state.canStop;
    
    // Show reason if disabled
    if (!state.canStartProofing) {
        showTooltip('proof-now', 'Cannot start: already active or in settings');
    }
}
```

## Testing Strategy

1. **Display Tests:**
   - Start proof from web → Display shows proofing screen with timer
   - Stop proof from web → Display returns to menu
   - Start proof from device → Web shows updated state
   - Navigate to settings → Proof button disabled on web

2. **Web Tests:**
   - Start proof from device → Web updates immediately
   - Temperature changes → Web shows new temperature
   - Mode changes → Web shows new mode
   - Screen changes → Web disables appropriate buttons

3. **Sync Tests:**
   - Rapid actions from both sources
   - State changes while viewing different screens
   - Recovery from error conditions

## Success Criteria

- [ ] Display always shows correct content when action from web
- [ ] Web always shows correct state when action from display
- [ ] Impossible actions are disabled on web
- [ ] Both inputs work consistently
- [ ] No race conditions or sync issues
- [ ] Code compiles and runs without errors

## Notes

- Prioritize making existing functionality work correctly
- Then add validation and improvements
- Test incrementally at each step
- Don't break working features while fixing others
