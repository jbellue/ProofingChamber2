#include "SimpleTime.h"
#include "MenuActions.h"
#include "DebugUtils.h"

MenuActions::MenuActions(AppContext* ctx, AdjustValue* adjustValue, 
        AdjustTime* adjustTime, ProofingController* ProofingController, CoolingScreen* coolingScreen,
        WiFiResetController* wifiResetController, SetTimezoneController* setTimezoneController, RebootController* rebootController) :
    _ctx(ctx),
    _rebootController(rebootController),
    _adjustValue(adjustValue),
    _adjustTime(adjustTime),
    _proofingController(ProofingController),
    _coolingScreen(coolingScreen),
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
    if (!_ctx || !_ctx->screens || !_adjustTime || !_coolingScreen) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustTime);

    // Lambda calculates end time based on user input from AdjustTime
    auto timeCalculator = [this]() -> time_t {
        struct tm timeinfo = _adjustTime->getTime();
        const time_t delayInSeconds = timeinfo.tm_mday * 24 * 60 * 60 + timeinfo.tm_hour * 60 * 60 + timeinfo.tm_min * 60;
        struct tm now;
        getLocalTime(&now);
        const time_t now_time = mktime(&now);
        const time_t endTime = now_time + delayInSeconds;
        DEBUG_PRINT("end time in:");
        DEBUG_PRINTLN(endTime);
        return endTime;
    };
    // Prepare screens for deferred begin via ScreensManager
    _coolingScreen->prepare(timeCalculator, _proofingController, menu);
    SimpleTime startTime(0, 0, 0);
    _adjustTime->prepare("Pousser dans...", _coolingScreen, menu, startTime);
}

void MenuActions::proofAtAction() {
    if (!_ctx || !_ctx->screens || !_adjustTime || !_coolingScreen) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustTime);
    struct tm timeinfo;
    SimpleTime startTime(0, 0, 0);
    if (!getLocalTime(&timeinfo)) {
        DEBUG_PRINTLN("Failed to obtain time, defaulting to 0:00");
        _adjustTime->prepare("Pousser \xC3\xA0...", _coolingScreen, menu, startTime);
    }
    startTime.hours = timeinfo.tm_hour;
    startTime.minutes = timeinfo.tm_min;
    // Lambda calculates end time based on user input from AdjustTime
    auto timeCalculator = [this]() -> time_t {
        struct tm timeinfo = _adjustTime->getTime();
        struct tm endTime;
        getLocalTime(&endTime);
        endTime.tm_mday += timeinfo.tm_mday;
        endTime.tm_hour = timeinfo.tm_hour;
        endTime.tm_min = timeinfo.tm_min;
        endTime.tm_sec = 0;
        DEBUG_PRINT("end time in:");
        DEBUG_PRINTLN(mktime(&endTime));
        return mktime(&endTime);
    };
    _coolingScreen->prepare(timeCalculator, _proofingController, menu); // Pass menu screen
    _adjustTime->prepare("Pousser \xC3\xA0...", _coolingScreen, menu, startTime);
}

void MenuActions::adjustHotTargetTemp() {
    if (!_ctx || !_ctx->screens || !_adjustValue) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->prepare("Temp\xC3\xA9rature\n" "de chauffe vis\xC3\xA9" "e", "/hot/target_temp.txt");
}

void MenuActions::adjustHotLowerLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValue) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->prepare("Limite basse\n" "de chauffe", "/hot/lower_limit.txt");
}

void MenuActions::adjustHotHigherLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValue) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->prepare("Limite haute\n" "de chauffe", "/hot/higher_limit.txt");
}

void MenuActions::adjustColdTargetTemp() {
    if (!_ctx || !_ctx->screens || !_adjustValue) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->prepare("Temp\xC3\xA9rature\n" "de froid vis\xC3\xA9" "e", "/cold/target_temp.txt");
}

void MenuActions::adjustColdLowerLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValue) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->prepare("Limite basse\n" "de froid", "/cold/lower_limit.txt");
}

void MenuActions::adjustColdHigherLimit() {
    if (!_ctx || !_ctx->screens || !_adjustValue) return;
    Screen* menu = _ctx->screens->getActiveScreen();
    if (!menu) return;
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->prepare("Limite haute\n" "de froid", "/cold/higher_limit.txt");
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
