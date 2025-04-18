#include <WiFiManager.h>
#include "SimpleTime.h"
#include "MenuActions.h"
#include "DebugUtils.h"

MenuActions::MenuActions(ScreensManager* screensManager, AdjustValue* adjustValue, 
        AdjustTime* adjustTime,ProofingScreen* proofingScreen, CoolingScreen* coolingScreen,
        WiFiReset* wifiReset, SetTimezone* setTimezone, Reboot* reboot) :
    _reboot(reboot),
    _screensManager(screensManager),
    _adjustValue(adjustValue),
    _adjustTime(adjustTime),
    _proofingScreen(proofingScreen),
    _coolingScreen(coolingScreen),
    _wifiReset(wifiReset),
    _setTimezone(setTimezone)
{}

void MenuActions::proofNowAction() {
    DEBUG_PRINTLN("MenuActions: proofNowAction called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_proofingScreen);
    _proofingScreen->setNextScreen(menu);
    _proofingScreen->begin();
}

void MenuActions::proofInAction() {
    DEBUG_PRINTLN("MenuActions: proofInAction called");
    Screen* menu = _screensManager->getActiveScreen();
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
    _coolingScreen->begin(timeCalculator, _proofingScreen, menu); // Pass menu screen
    SimpleTime startTime(0, 0, 0);
    _adjustTime->begin("Pousser dans...", _coolingScreen, menu, startTime);
}

void MenuActions::proofAtAction() {
    DEBUG_PRINTLN("MenuActions: proofAtAction called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustTime);
    struct tm timeinfo;
    SimpleTime startTime(0, 0, 0);
    if (!getLocalTime(&timeinfo)) {
        DEBUG_PRINTLN("Failed to obtain time, defaulting to 0:00");
        _adjustTime->begin("Pousser \xC3\xA0...", _coolingScreen, menu, startTime);
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
    _coolingScreen->begin(timeCalculator, _proofingScreen, menu); // Pass menu screen
    _adjustTime->begin("Pousser \xC3\xA0...", _coolingScreen, menu, startTime);
}

void MenuActions::adjustHotTargetTemp() {
    DEBUG_PRINTLN("MenuActions: adjustHotTargetTemp called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->begin("Temp\xC3\xA9rature\n" "de chauffe vis\xC3\xA9" "e", "/hot/target_temp.txt");
}

void MenuActions::adjustHotLowerLimit() {
    DEBUG_PRINTLN("MenuActions: adjustHotLowerLimit called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->begin("Limite basse\n" "de chauffe", "/hot/lower_limit.txt");
}

void MenuActions::adjustHotHigherLimit() {
    DEBUG_PRINTLN("MenuActions: adjustHotHigherLimit called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->begin("Limite haute\n" "de chauffe", "/hot/higher_limit.txt");
}

void MenuActions::adjustColdTargetTemp() {
    DEBUG_PRINTLN("MenuActions: adjustColdTargetTemp called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->begin("Temp\xC3\xA9rature\n" "de froid vis\xC3\xA9" "e", "/cold/target_temp.txt");
}

void MenuActions::adjustColdLowerLimit() {
    DEBUG_PRINTLN("MenuActions: adjustColdLowerLimit called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->begin("Limite basse\n" "de froid", "/cold/lower_limit.txt");
}

void MenuActions::adjustColdHigherLimit() {
    DEBUG_PRINTLN("MenuActions: adjustColdHigherLimit called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustValue);
    _adjustValue->setNextScreen(menu);
    _adjustValue->begin("Limite haute\n" "de froid", "/cold/higher_limit.txt");
}

void MenuActions::resetWiFiAndReboot() {
    DEBUG_PRINTLN("MenuActions: resetWiFiAndReboot called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_wifiReset);
    _wifiReset->setNextScreen(menu);
    _wifiReset->begin();
}

void MenuActions::reboot() {
    DEBUG_PRINTLN("MenuActions: reboot called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_reboot);
    _reboot->setNextScreen(menu);
    _reboot->begin();
}

void MenuActions::adjustTimezone() {
    DEBUG_PRINTLN("MenuActions: adjustTimezone called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_setTimezone);
    _setTimezone->setNextScreen(menu);
    _setTimezone->begin();
}
