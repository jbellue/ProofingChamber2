#include <WiFiManager.h>
#include "MenuActions.h"
#include "DebugUtils.h"

MenuActions::MenuActions(ScreensManager* screensManager, AdjustValue* adjustValue, AdjustTime* adjustTime,
                        ProofingScreen* proofingScreen, WiFiReset* wifiReset, SetTimezone* setTimezone) :
    _screensManager(screensManager),
    _adjustValue(adjustValue),
    _adjustTime(adjustTime),
    _proofingScreen(proofingScreen),
    _wifiReset(wifiReset),
    _setTimezone(setTimezone)
{}

void MenuActions::proofNowAction() {
    DEBUG_PRINTLN("MenuActions: proofNowAction called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_proofingScreen);
    _proofingScreen->setNextScreen(menu);
    _proofingScreen->begin("9:34", 0, true, false);
}

void MenuActions::proofInAction() {
    DEBUG_PRINTLN("MenuActions: proofInAction called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustTime);
    _adjustTime->setNextScreen(menu); // TODO go to screen cooling
    _adjustTime->begin("Pousser dans...");
}

void MenuActions::proofAtAction() {
    DEBUG_PRINTLN("MenuActions: proofAtAction called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_adjustTime);
    _adjustTime->setNextScreen(menu); // TODO go to screen cooling
    struct tm timeinfo;
    int hour, minute;
    if (!getLocalTime(&timeinfo)) {
        DEBUG_PRINTLN("Failed to obtain time, defaulting to 0:00");
        _adjustTime->begin("Pousser \xC3\xA0...");
    }
    _adjustTime->begin("Pousser \xC3\xA0...", timeinfo.tm_hour, timeinfo.tm_min);
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

void MenuActions::adjustTimezone() {
    DEBUG_PRINTLN("MenuActions: adjustTimezone called");
    Screen* menu = _screensManager->getActiveScreen();
    menu->setNextScreen(_setTimezone);
    _setTimezone->setNextScreen(menu);
    _setTimezone->begin();
}