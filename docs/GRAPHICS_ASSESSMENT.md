# Graphics Architecture Assessment - ProofingChamber2

**Date**: 2026-01-26  
**Author**: GitHub Copilot  
**Purpose**: Assess current graphics handling and evaluate optimization opportunities

## Executive Summary

The current graphics implementation using **U8G2 library with full-frame buffering** is appropriate for this ESP32-C3 project with a 128x64 OLED display. While **LVGL was considered**, it is **not recommended** due to excessive memory overhead and unnecessary complexity for this simple monochrome UI.

However, there are **optimization opportunities** in the current implementation that can improve performance without library changes.

---

## Current Architecture

### Graphics Library
- **Library**: U8G2 (latest version)
- **Display**: SH1106 128x64 OLED (monochrome)
- **Interface**: Hardware I2C
- **Buffer Mode**: Full-frame buffer (`U8G2_SH1106_128X64_NONAME_F_HW_I2C`)

### Buffer Mode Analysis

**Current**: Full-frame buffer mode (`_F_` suffix)
- **Buffer Size**: 128×64÷8 = **1,024 bytes (1 KB)**
- **Memory Impact**: 0.25% of available 400KB RAM
- **Advantages**: 
  - No flicker
  - Full control over all pixels
  - Simplified drawing logic
- **Disadvantages**:
  - Always sends 1KB to display via I2C, even for small updates
  - No built-in dirty region tracking

**Alternative**: Page mode (`_1_` or `_2_` suffix)
- **Buffer Size**: 128 bytes (1 page) or 256 bytes (2 pages)
- **Advantages**:
  - Much smaller RAM footprint
  - Better for memory-constrained scenarios
- **Disadvantages**:
  - Must redraw on each page iteration
  - More complex drawing code
  - Potential flicker if not carefully managed
  - Not needed for ESP32-C3 with 400KB RAM

### Drawing Patterns

The codebase follows a **partial redraw pattern**:

```cpp
// Pattern used across views
clearBuffer()           // Clear entire buffer
[draw operations]       // Draw everything
sendBuffer()           // Send entire 1KB to display

// Optimization attempt in views
if (valueChanged) {
    drawPartialUpdate();  // Clear only changed region
    return true;         // Signal redraw needed
}
return false;           // No redraw needed
```

**Example**: `DataDisplayView::drawTemperature()`
```cpp
if (abs(_lastTemperature - temperatureC) < 0.1) {
    return false; // Skip redraw
}
// Clear only temp region, redraw temp
_display->setDrawColor(0);
_display->drawBox(tempX, tempY - ascent, tempWidth, tempHeight);
_display->setDrawColor(1);
_display->drawUTF8(tempX, tempY, tempBuffer);
return true;
```

---

## Inefficiencies Identified

### 1. **Menu Screen - Redundant Full Clears**
**Location**: `src/screens/Menu.cpp:142-143`

**Issue**: Every animation frame clears the entire buffer:
```cpp
if (redraw) {
    drawMenu();  // Calls clearBuffer() → draws everything → sendBuffer()
}
```

**Impact**: During smooth scrolling, this runs every frame (~60 FPS target), sending 1KB over I2C each time.

**Optimization**: Only clear and redraw changed regions, or reduce animation frame rate.

---

### 2. **Whole Buffer Transmission**
**Location**: All `sendBuffer()` calls

**Issue**: U8G2's `sendBuffer()` always sends the entire 1KB frame buffer to the display, even if only a few pixels changed.

**Impact**: 
- I2C bandwidth: 1024 bytes × (I2C speed / 8) = transmission time
- At 400kHz I2C: ~20ms per frame
- At 100kHz I2C: ~80ms per frame

**Optimization**: 
- Not directly fixable with U8G2's API
- Could implement custom dirty region tracking at application level
- U8G2 does not expose partial buffer transmission

---

### 3. **Font Metrics Recalculation**
**Location**: Multiple views (e.g., `DataDisplayView.cpp:28-29`, `ProofingView.cpp`)

**Issue**: Same font metrics calculated repeatedly:
```cpp
const uint8_t ascent = _display->getAscent();
const uint8_t descent = _display->getDescent();
```

**Impact**: Minor, but adds unnecessary function calls.

**Optimization**: Cache font metrics after `setFont()` if font doesn't change frequently.

---

### 4. **DisplayManager Update Method Unused**
**Location**: `src/DisplayManager.cpp:9-14`

**Issue**: `update()` method implements page-mode loop but is never called:
```cpp
void DisplayManager::update() {
    _display.firstPage();
    do {
        // Draw the current page
    } while (_display.nextPage());
}
```

**Impact**: Dead code, confusing.

**Optimization**: Remove or document purpose.

---

## LVGL Evaluation

### Why LVGL Was Considered
- Modern, feature-rich GUI library
- Popular in embedded systems
- Supports animations, widgets, touch interfaces

### Why LVGL Is **NOT Recommended**

| Criteria | U8G2 | LVGL | Verdict |
|----------|------|------|---------|
| **Memory Footprint** | 1 KB buffer | 40-50 KB minimum | ❌ 40-50× larger |
| **RAM Available** | 400 KB total | 400 KB total | ❌ 10-12% just for UI |
| **SH1106 Support** | Native driver | Custom driver needed | ❌ Extra work |
| **Integration Complexity** | ✅ Already done | Rewrite all views | ❌ High effort |
| **Feature Fit** | Perfect for simple UI | Overkill | ❌ Unnecessary |
| **Dependencies** | Lightweight | Heavy | ❌ Bloat |

**Conclusion**: LVGL is designed for **color touchscreens** with **complex UIs**. This project has:
- **Monochrome 128x64 display**
- **Simple rotary encoder input**
- **Text, icons, and line graphs only**

U8G2 is the **optimal choice** for this use case.

---

## Recommended Optimizations

### Priority 1: Reduce Menu Animation Overhead ⚡

**Problem**: Menu redraws entire screen every animation frame.

**Solution**: Implement frame-rate limiting or reduce animation smoothness.

**Implementation**:
```cpp
// Option A: Reduce animation speed (fewer frames)
const float ANIMATION_SPEED = 0.1; // Current: 0.3

// Option B: Skip sendBuffer() if minimal change
if (redraw && pixelDeltaSignificant) {
    drawMenu();
}
```

**Impact**: ~50% reduction in I2C traffic during menu scrolling.

---

### Priority 2: Remove Dead Code 🧹

**Problem**: `DisplayManager::update()` is never called.

**Solution**: 
- Option A: Remove the method
- Option B: Document that it's for future page-mode support

**Implementation**: Add comment or remove.

---

### Priority 3: Cache Font Metrics 📊 - REMOVED

**Problem**: Repeated `getAscent()`/`getDescent()` calls.

**Initial Solution**: Cache after `setFont()` in DisplayManager.

**Issue Identified**: Cannot guarantee cache validity because:
- `getDisplay()` exposes underlying U8G2 object
- Graph class uses direct U8G2 access
- U8G2 doesn't provide `getFont()` to verify current font
- Cache could become stale if font set directly on U8G2 object

**Final Decision**: Remove caching to prioritize correctness over minor performance gain.

**Rationale**: This is a safety-critical system (heating/cooling control). Correctness > micro-optimization.

---

### Priority 4: Consider Partial Buffer Mode (Future) 🔮

**Problem**: Full 1KB buffer not strictly necessary for 400KB RAM device, but beneficial.

**Solution**: Only if memory becomes constrained in future features:
- Switch to `U8G2_SH1106_128X64_NONAME_1_HW_I2C` (128-byte buffer)
- Refactor views to support page-mode rendering

**Implementation**: **NOT RECOMMENDED NOW** - current full buffer is fine for ESP32-C3.

---

## Conclusion

### Keep U8G2 ✅
- Perfect fit for this project
- Mature, stable, well-supported
- Minimal memory overhead
- Already integrated

### Skip LVGL ❌
- Massive memory overhead
- Unnecessary complexity
- No tangible benefits for monochrome 128×64 display

### Implement Minor Optimizations ⚡
1. Optimize Menu animation frame rate
2. Clean up dead code
3. Cache font metrics where repeated

### Current Architecture is Sound 🎯
The current implementation is well-structured with:
- Clean abstraction via `IDisplayManager`
- Proper separation of views and controllers
- Effective partial redraw logic
- Good memory utilization

**No major refactoring needed.** Focus on **incremental improvements** rather than wholesale replacement.

---

## References

- [U8G2 Documentation](https://github.com/olikraus/u8g2/wiki)
- [LVGL Documentation](https://docs.lvgl.io/)
- [ESP32-C3 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
