#include "SimpleTime.h"
#include "MenuActions.h"
#include "DebugUtils.h"
#include "screens/controllers/AdjustTimeController.h"
#include "screens/Menu.h"
#include "Timezones.h"

// Static member definitions
SimpleTime MenuActions::s_proofInTime(0, 0, 0);
SimpleTime MenuActions::s_proofAtTime(0, 0, 0);

MenuActions::MenuActions(AppContext* ctx, AdjustValueController* adjustValueController, 
        AdjustTimeController* adjustTimeController, ProofingController* ProofingController, CoolingController* coolingController,
        WiFiResetController* wifiResetController, RebootController* rebootController, DataDisplayController* dataDisplayController, ConfirmTimezoneController* confirmTimezoneController) :
    _ctx(ctx),
    _rebootController(rebootController),
    _adjustValueController(adjustValueController),
    _adjustTimeController(adjustTimeController),
    _proofingController(ProofingController),
    _coolingController(coolingController),
    _wifiResetController(wifiResetController),
    _dataDisplayController(dataDisplayController),
    _confirmTimezoneController(confirmTimezoneController)
{}

void MenuActions::proofNowAction() {
    if (!_ctx || !_ctx->screens || !_proofingController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_proofingController);
    _proofingController->setNextScreen(menu);
}

void MenuActions::proofInAction() {
    if (!_ctx || !_ctx->screens || !_adjustTimeController || !_coolingController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustTimeController);

    // Prepare screens for deferred begin via ScreensManager
    _coolingController->prepare(&calculateProofInEndTime, _proofingController, menu);
    SimpleTime startTime(0, 0, 0);
    _adjustTimeController->prepare("Pousser dans...", _coolingController, menu, startTime, TimeMode::ProofIn);
}

void MenuActions::proofAtAction() {
    if (!_ctx || !_ctx->screens || !_adjustTimeController || !_coolingController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustTimeController);
    struct tm timeinfo;
    SimpleTime startTime(0, 0, 0);
    if (!getLocalTime(&timeinfo)) {
        DEBUG_PRINTLN("Failed to obtain time, defaulting to 0:00");
        _adjustTimeController->prepare("Pousser \xC3\xA0...", _coolingController, menu, startTime, TimeMode::ProofAt);
    }
    startTime.hours = timeinfo.tm_hour;
    startTime.minutes = timeinfo.tm_min;
    
    _coolingController->prepare(&calculateProofAtEndTime, _proofingController, menu);
    _adjustTimeController->prepare("Pousser \xC3\xA0...", _coolingController, menu, startTime, TimeMode::ProofAt);
}

void MenuActions::adjustHotLowerLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Limite basse\n" "de chauffe", "/hot/lower_limit.txt");
}

void MenuActions::adjustHotHigherLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Limite haute\n" "de chauffe", "/hot/higher_limit.txt");
}

void MenuActions::adjustColdLowerLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Limite basse\n" "de froid", "/cold/lower_limit.txt");
}

void MenuActions::adjustColdHigherLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Limite haute\n" "de froid", "/cold/higher_limit.txt");
}

void MenuActions::resetWiFiAndReboot() {
    if (!_ctx || !_ctx->screens || !_wifiResetController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_wifiResetController);
    _wifiResetController->setNextScreen(menu);
}

void MenuActions::reboot() {
    if (!_ctx || !_ctx->screens || !_rebootController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_rebootController);
    _rebootController->setNextScreen(menu);
}

void MenuActions::showDataDisplay() {
    if (!_ctx || !_ctx->screens || !_dataDisplayController) return;
    BaseController* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_dataDisplayController);
    _dataDisplayController->setNextScreen(menu);
}

// Static callback functions for time calculations
time_t MenuActions::calculateProofInEndTime() {
    // Convert stored time to seconds delay from now
    struct tm now;
    getLocalTime(&now);
    time_t nowTime = mktime(&now);
    time_t delayInSeconds =
        s_proofInTime.days * 86400 +
        s_proofInTime.hours * 3600 +
        s_proofInTime.minutes * 60;
    return nowTime + delayInSeconds;
}

time_t MenuActions::calculateProofAtEndTime() {
    // Convert stored time to target time_t
    struct tm targetTime;
    getLocalTime(&targetTime);

    targetTime.tm_mday += s_proofAtTime.days;
    targetTime.tm_hour = s_proofAtTime.hours;
    targetTime.tm_min = s_proofAtTime.minutes;
    targetTime.tm_sec = 0;
    // Normalize date in case of overflow
    return mktime(&targetTime);
}

void MenuActions::saveTimezone(const char* posixString) {
    if (!_ctx || !_ctx->storage) return;
    _ctx->storage->writeString("/timezone.txt", posixString);
    DEBUG_PRINT("Timezone saved: ");
    DEBUG_PRINTLN(posixString);
}

// Generic timezone selection handler - uses Menu context to determine which timezone was selected
void MenuActions::selectTimezoneByData() {
    if (!_menu) {
        DEBUG_PRINTLN("Menu context not set");
        return;
    }
    
    // Get the current menu and selected index
    Menu::MenuItem* currentMenu = _menu->getCurrentMenu();
    uint8_t selectedIndex = _menu->getCurrentMenuIndex();
    
    if (!currentMenu) {
        DEBUG_PRINTLN("Invalid menu context");
        return;
    }
    
    // Find which continent menu we're in by matching the pointer
    extern Menu::MenuItem* timezoneMenu;
    
    int continentIndex = -1;
    int continentCount = timezones::getContinentCount();
    
    for (int c = 0; c < continentCount; c++) {
        if (timezoneMenu && timezoneMenu[c].subMenu == currentMenu) {
            continentIndex = c;
            break;
        }
    }
    
    if (continentIndex < 0) {
        DEBUG_PRINTLN("Could not determine continent from menu context");
        return;
    }
    
    const char* continentName = timezones::getContinentName(continentIndex);
    int tzCount = timezones::getTimezoneCount(continentName);
    
    // The last item is "Retour", so if we selected it, don't do anything
    if (selectedIndex >= tzCount) {
        return;
    }
    
    // Show confirmation dialog
    if (!_ctx || !_ctx->screens || !_confirmTimezoneController) {
        return;
    }
    
    const timezones::Timezone* tz = timezones::getTimezone(continentName, selectedIndex);
    if (!tz) {
        DEBUG_PRINTLN("Could not get timezone");
        return;
    }
    
    _confirmTimezoneController->setTimezoneInfo(continentName, tz->name, tz->posixString);
    
    BaseController* currentScreen = _ctx->screens->getActiveScreen();
    if (currentScreen) {
        DEBUG_PRINTLN("Navigating to ConfirmTimezoneController");
        currentScreen->setNextScreen(_confirmTimezoneController);
        _confirmTimezoneController->setNextScreen(currentScreen);
        // No forced transition; let ScreensManager switch after action returns
    }
}
