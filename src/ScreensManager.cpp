#include "ScreensManager.h"
#include "DebugUtils.h"

ScreensManager::ScreensManager() : activeScreen(nullptr) {}

void ScreensManager::setActiveScreen(BaseController* screen) {
    activeScreen = screen;
    if (activeScreen) {
        activeScreen->begin();
    }
}

void ScreensManager::update() {
    if (activeScreen) {
        if (!activeScreen->update(false)) {
            DEBUG_PRINTLN("ScreensManager: Active screen finished. Transitioning to next screen.");
            activeScreen = activeScreen->getNextScreen();
            if (activeScreen) {
                activeScreen->begin();
                activeScreen->update(true);
            }
        }
    }
}

bool ScreensManager::isScreenActive() const {
    return activeScreen != nullptr;
}
