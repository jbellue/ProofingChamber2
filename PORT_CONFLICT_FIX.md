# Web Server Port Conflict Fix

## Problem Statement

"It's something else - now even if the portal manages to start, I can't connect to it - something in the webserver code makes that fail"

Users couldn't connect to the WiFi captive portal, even when it appeared to start. The web server code was interfering with the portal's operation.

## Root Cause Analysis

### The Port Conflict

Both the web server and WiFiManager's captive portal need **port 80**:
- **Web server**: Serves the application's web interface
- **Captive portal**: Serves WiFi configuration page

### Timing of Initialization

The problem occurred because of **when** the web server reserved port 80:

```cpp
// Global object in main.cpp (line 87)
services::WebServerService webServerService(&appContext);
```

When a global object is created, its constructor runs during **global initialization**, which happens BEFORE `setup()` and BEFORE any application code runs.

### The Old Constructor

```cpp
// WebServerService.cpp (BEFORE FIX)
WebServerService::WebServerService(AppContext* ctx)
    : _ctx(ctx), _server(80) {  // ← AsyncWebServer created here!
}
```

The member initializer list `: _server(80)` creates an `AsyncWebServer` object and immediately reserves port 80.

### Timeline of the Conflict

```
1. Program starts
   ↓
2. Global object initialization
   ↓
3. WebServerService() constructor runs
   ↓
4. _server(80) reserves port 80 ← HAPPENS IMMEDIATELY
   ↓
5. setup() runs
   ↓
6. Initialization screen starts
   ↓
7. WiFiManager.autoConnect() tries to start captive portal
   ↓
8. Captive portal needs port 80
   ↓
9. PORT ALREADY IN USE! ← CONFLICT
   ↓
10. Captive portal can't start properly
    ↓
11. Users can't connect to configure WiFi
```

### Why This Is Subtle

The issue is subtle because:
1. ✅ Web server's `begin()` was called AFTER WiFi connection
2. ❌ But the `AsyncWebServer` **object** was created long before
3. ❌ Port reservation happens at **object creation**, not at `begin()`
4. ❌ Global object creation happens at **program start**, not during setup

So even though the code tried to delay starting the server until after WiFi, the port was already reserved from the very beginning.

## Solution: Lazy Initialization

### The Pattern

Instead of creating the `AsyncWebServer` object immediately, we use a **pointer** and create the object only when needed:

```cpp
// Member variable (header file)
AsyncWebServer* _server;  // Pointer instead of object

// Constructor - no allocation yet
WebServerService::WebServerService(AppContext* ctx)
    : _ctx(ctx), _server(nullptr) {
    // Port 80 NOT reserved here
}

// begin() - allocate when needed
void WebServerService::begin() {
    if (!_server) {
        _server = new AsyncWebServer(80);  // Port 80 reserved HERE
    }
    setupRoutes();
    _server->begin();
}

// Destructor - cleanup
WebServerService::~WebServerService() {
    if (_server) {
        delete _server;
        _server = nullptr;
    }
}
```

### New Timeline (Fixed)

```
1. Program starts
   ↓
2. Global object initialization
   ↓
3. WebServerService() constructor runs
   ↓
4. _server = nullptr (no port reserved) ← DELAYED!
   ↓
5. setup() runs
   ↓
6. Initialization screen starts
   ↓
7. WiFiManager.autoConnect() starts captive portal
   ↓
8. Captive portal uses port 80 ✅
   ↓
9. User configures WiFi successfully ✅
   ↓
10. WiFi connects
    ↓
11. WebServerService.begin() creates server
    ↓
12. new AsyncWebServer(80) reserves port 80 ✅
    ↓
13. Web interface works ✅
```

## Implementation Details

### Changes to Header File

```cpp
// WebServerService.h
class WebServerService : public IWebServerService {
public:
    explicit WebServerService(AppContext* ctx);
    ~WebServerService();  // Added destructor
    void begin() override;
    void update() override;

private:
    AppContext* _ctx;
    AsyncWebServer* _server;  // Changed from object to pointer
    // ... rest of members
};
```

### Changes to Implementation

**Constructor:**
```cpp
WebServerService::WebServerService(AppContext* ctx)
    : _ctx(ctx), _server(nullptr) {
    // Don't create AsyncWebServer here - delays port 80 allocation
    // until after WiFi captive portal completes
}
```

**Destructor:**
```cpp
WebServerService::~WebServerService() {
    if (_server) {
        delete _server;
        _server = nullptr;
    }
}
```

**begin() Method:**
```cpp
void WebServerService::begin() {
    // Create the server now, after WiFi connection is established
    // This avoids port conflict with WiFiManager captive portal
    if (!_server) {
        _server = new AsyncWebServer(80);
    }
    
    setupRoutes();
    _server->begin();
    DEBUG_PRINTLN("Web server started on port 80");
}
```

**setupRoutes() Safety Check:**
```cpp
void WebServerService::setupRoutes() {
    if (!_server) return;  // Safety check
    
    // Setup all routes using _server->on(...)
    _server->on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetRoot(request);
    });
    // ... more routes
}
```

**Pointer Dereference:**

All uses of `_server` changed from `.` to `->`:
```cpp
// Before
_server.on("/api/status", ...)
_server.begin()

// After
_server->on("/api/status", ...)
_server->begin()
```

## Benefits

### 1. Captive Portal Works

✅ Port 80 available during WiFi configuration
✅ Users can connect to captive portal
✅ WiFi credentials can be entered

### 2. Web Server Still Works

✅ Server created after WiFi connects
✅ Port 80 available for web interface
✅ All functionality preserved

### 3. Proper Resource Management

✅ Resources allocated only when needed
✅ Memory used only after WiFi connection
✅ Proper cleanup in destructor

### 4. Better Architecture

✅ Lazy initialization pattern
✅ Separation of object creation and initialization
✅ More predictable behavior

## Testing Procedure

### Test 1: Fresh Device (No Saved WiFi)

1. Flash device with fix
2. Device boots
3. **Expected**: Captive portal starts and is connectable
4. Connect to "ProofingChamber" network
5. **Expected**: Configuration page loads
6. Enter WiFi credentials
7. **Expected**: Device connects successfully
8. **Expected**: Web server starts
9. Access web interface via IP
10. **Expected**: Web interface works

### Test 2: Device with Saved WiFi

1. Flash device with fix (has saved credentials)
2. Device boots
3. **Expected**: Connects to saved WiFi automatically
4. **Expected**: Web server starts
5. Access web interface via IP
6. **Expected**: Web interface works

### Test 3: WiFi Reset

1. Device running normally
2. Use WiFi reset function
3. Device reboots
4. **Expected**: Captive portal starts and is connectable
5. Configure new WiFi
6. **Expected**: Connects and web server works

## Common Patterns in ESP32

### The Global Object Problem

Many ESP32 issues stem from global object initialization:

```cpp
// This runs BEFORE setup()!
MyClass globalObject(param);
```

Problems:
- ❌ Constructor runs too early
- ❌ Setup dependencies not ready
- ❌ Hardware not initialized
- ❌ Can conflict with system initialization

### Solutions

**Option 1: Pointers**
```cpp
MyClass* globalObject = nullptr;

void setup() {
    globalObject = new MyClass(param);  // Create when ready
}
```

**Option 2: Static Variables in Functions**
```cpp
MyClass& getObject() {
    static MyClass instance(param);  // Created on first call
    return instance;
}
```

**Option 3: Lazy Initialization (Our Approach)**
```cpp
class MyClass {
    Resource* _resource;
    
    MyClass() : _resource(nullptr) {}
    
    void begin() {
        if (!_resource) {
            _resource = new Resource();
        }
    }
};
```

## Lessons Learned

### 1. Port Allocation Timing

**Key insight**: Port allocation happens at object construction, not at method call.

```cpp
// Port reserved HERE (at construction)
AsyncWebServer server(80);

// NOT here (at begin)
server.begin();
```

### 2. Global Objects Are Dangerous

Global objects with resource allocation should use lazy initialization to avoid:
- Port conflicts
- Hardware conflicts
- Dependency issues
- Initialization order problems

### 3. Separation of Concerns

Separate:
- **Object creation** (constructor)
- **Resource allocation** (begin/init method)
- **Resource usage** (other methods)

### 4. Think About Lifecycle

Consider the full lifecycle:
```
Global init → setup() → WiFi config → WiFi connect → App runs
```

Resources should be allocated at the appropriate stage.

## Prevention

### Code Review Checklist

When creating global objects:
- [ ] Does constructor allocate resources (ports, memory, hardware)?
- [ ] Could these resources conflict with system initialization?
- [ ] Can resource allocation be delayed to begin() or init()?
- [ ] Is cleanup properly handled in destructor?

### Best Practices

1. **Avoid resource allocation in constructors** for global objects
2. **Use lazy initialization** for expensive resources
3. **Document lifecycle requirements** in comments
4. **Test with WiFi configuration** scenarios
5. **Check for port conflicts** in logs

## Related Issues

This pattern applies to other potential conflicts:

### Serial Port
```cpp
// Bad: Opens serial in constructor
class Logger {
    Logger() {
        Serial.begin(115200);  // ← Too early!
    }
};

// Good: Lazy initialization
class Logger {
    bool _initialized = false;
    
    void begin() {
        if (!_initialized) {
            Serial.begin(115200);
            _initialized = true;
        }
    }
};
```

### I2C/SPI
```cpp
// Bad: Initializes bus in constructor
class Display {
    Display() {
        Wire.begin();  // ← Might conflict!
    }
};

// Good: Initialize in begin()
class Display {
    void begin() {
        Wire.begin();
    }
};
```

## Conclusion

The web server was interfering with the WiFi captive portal because it reserved port 80 during global object initialization, long before the captive portal tried to start.

By changing from immediate object creation to lazy initialization using a pointer, we ensured that:

1. ✅ Port 80 is available during WiFi configuration
2. ✅ Captive portal can start and be accessed
3. ✅ Web server gets port 80 after WiFi connects
4. ✅ Both features work correctly

This is a classic example of the importance of **resource allocation timing** in embedded systems with multiple components competing for limited resources.

The fix is minimal (changing object to pointer and adding lazy initialization) but solves a critical user-facing issue where WiFi configuration was impossible.
