#pragma once

#include "InputManager.h"

class Screen {
public:
    virtual ~Screen() = default;

    // Initialize the screen
    template <typename... Args>
    void begin(Args&&... args) {
        beginImpl(std::forward<Args>(args)...);
    }

    // Update the screen (e.g., handle input, redraw)
    virtual bool update(bool forceRedraw) = 0;

    // Get the next screen
    Screen* getNextScreen() const { return _nextScreen; }

    // Set the next screen
    void setNextScreen(Screen* screen) { _nextScreen = screen; }

protected:
    virtual void beginImpl() = 0;

    template <typename T, typename... Args>
    void beginImpl(T&& first, Args&&... rest) {
        beginImpl(std::forward<T>(first), std::forward<Args>(rest)...);
    }

private:
    Screen* _nextScreen = nullptr; // Pointer to the next screen
};
