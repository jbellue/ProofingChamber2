#include "ScreensManager.h"
#include "DebugUtils.h"

ScreensManager::ScreensManager() : activeScreen(nullptr) {}

void ScreensManager::setActiveScreen(Screen* screen) {
    activeScreen = screen; // Set the active screen
    activeScreen->begin(); // Initialize the screen (if needed)
}

void ScreensManager::update() {
    if (activeScreen) {
        if (!activeScreen->update(false)) {
            DEBUG_PRINTLN("ScreensManager: Active screen finished. Transitioning to next screen.");
            activeScreen = activeScreen->getNextScreen(); // Transition to the next screen
            if (activeScreen) {
                activeScreen->begin(); // Initialize the new screen
                activeScreen->update(true);
            }
        }
    }
}

bool ScreensManager::isScreenActive() const {
    return activeScreen != nullptr;
}
