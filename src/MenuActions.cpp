#include "SimpleTime.h"
#include "MenuActions.h"
#include "DebugUtils.h"
#include "screens/controllers/AdjustTimeController.h"

// Static member definitions
SimpleTime MenuActions::s_proofInTime(0, 0, 0);
SimpleTime MenuActions::s_proofAtTime(0, 0, 0);

MenuActions::MenuActions(AppContext* ctx, AdjustValueController* adjustValueController, 
        AdjustTimeController* adjustTimeController, ProofingController* ProofingController, CoolingController* coolingController,
        WiFiResetController* wifiResetController, SetTimezoneController* setTimezoneController, RebootController* rebootController) :
    _ctx(ctx),
    _rebootController(rebootController),
    _adjustValueController(adjustValueController),
    _adjustTimeController(adjustTimeController),
    _proofingController(ProofingController),
    _coolingController(coolingController),
    _wifiResetController(wifiResetController),
    _setTimezoneController(setTimezoneController)
{}

void MenuActions::proofNowAction() {
    if (!_ctx || !_ctx->screens || !_proofingController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_proofingController);
    _proofingController->setNextScreen(menu);
}

void MenuActions::proofInAction() {
    if (!_ctx || !_ctx->screens || !_adjustTimeController || !_coolingController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustTimeController);

    // Prepare screens for deferred begin via ScreensManager
    _coolingController->prepare(&calculateProofInEndTime, _proofingController, menu);
    SimpleTime startTime(0, 0, 0);
    _adjustTimeController->prepare("Pousser dans...", _coolingController, menu, startTime, TimeMode::ProofIn);
}

void MenuActions::proofAtAction() {
    if (!_ctx || !_ctx->screens || !_adjustTimeController || !_coolingController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
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

void MenuActions::adjustHotTargetTemp() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Temp\xC3\xA9rature\n" "de chauffe vis\xC3\xA9" "e", "/hot/target_temp.txt");
}

void MenuActions::adjustHotLowerLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Limite basse\n" "de chauffe", "/hot/lower_limit.txt");
}

void MenuActions::adjustHotHigherLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Limite haute\n" "de chauffe", "/hot/higher_limit.txt");
}

void MenuActions::adjustColdTargetTemp() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Temp\xC3\xA9rature\n" "de froid vis\xC3\xA9" "e", "/cold/target_temp.txt");
}

void MenuActions::adjustColdLowerLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Limite basse\n" "de froid", "/cold/lower_limit.txt");
}

void MenuActions::adjustColdHigherLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValueController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValueController);
    _adjustValueController->setNextScreen(menu);
    _adjustValueController->prepare("Limite haute\n" "de froid", "/cold/higher_limit.txt");
}

void MenuActions::resetWiFiAndReboot() {
    if (!_ctx || !_ctx->screens || !_wifiResetController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_wifiResetController);
    _wifiResetController->setNextScreen(menu);
}

void MenuActions::reboot() {
    if (!_ctx || !_ctx->screens || !_rebootController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_rebootController);
    _rebootController->setNextScreen(menu);
}

void MenuActions::adjustTimezone() {
    if (!_ctx || !_ctx->screens || !_setTimezoneController) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_setTimezoneController);
    _setTimezoneController->setNextScreen(menu);
}

// Static callback functions for time calculations
time_t MenuActions::calculateProofInEndTime() {
    // Convert stored time to seconds delay from now
    struct tm now;
    getLocalTime(&now);
    time_t now_time = mktime(&now);
    
    // Calculate delay in seconds (simplified version)
    time_t delayInSeconds = s_proofInTime.hours * 3600 + s_proofInTime.minutes * 60;
    return now_time + delayInSeconds;
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
