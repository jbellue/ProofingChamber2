#pragma once

#include "../IInputManager.h"

// Forward declarations
struct AppContext;

/**
 * @brief Base class for all screen controllers in the application.
 * 
 * Provides common infrastructure for screen management and input handling:
 * - Automatic AppContext storage and access
 * - Screen navigation chain management
 * - Common encoder position reset on screen entry
 * 
 * All screen controllers should inherit from this base class.
 */
class BaseController {
public:
    explicit BaseController(AppContext* ctx);
    virtual ~BaseController() = default;

    // Initialize the screen
    template <typename... Args>
    void begin(Args&&... args) {
        beginImpl(std::forward<Args>(args)...);
    }

    // Update the screen (e.g., handle input, redraw)
    virtual bool update(bool forceRedraw) = 0;

    // Get the next screen
    BaseController* getNextScreen() const { return _nextScreen; }

    // Set the next screen
    void setNextScreen(BaseController* screen) { _nextScreen = screen; }
    
    // Get screen name for identification (for web interface)
    virtual const char* getScreenName() const { return "Unknown"; }

protected:
    virtual void beginImpl() = 0;

    template <typename T, typename... Args>
    void beginImpl(T&& first, Args&&... rest) {
        beginImpl(std::forward<T>(first), std::forward<Args>(rest)...);
    }

    /**
     * @brief Initialize input manager and reset encoder.
     * 
     * Call this at the start of your beginImpl() to ensure:
     * - Encoder position is reset to prevent carryover from previous screen
     */
    void initializeInputManager();
    
    // Accessors for derived classes
    IInputManager* getInputManager() const { return _inputManager; }
    AppContext* getContext() const { return _ctx; }

private:
    AppContext* _ctx;
    IInputManager* _inputManager;
    BaseController* _nextScreen = nullptr;
};
