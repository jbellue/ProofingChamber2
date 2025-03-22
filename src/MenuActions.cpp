#include <WiFiManager.h>
#include "MenuActions.h"
#include "DebugUtils.h"

MenuActions::MenuActions(ScreensManager* screensManager, AdjustValue* adjustValue, AdjustTime* adjustTime, ProofingScreen* proofingScreen) :
    _screensManager(screensManager),
    _adjustValue(adjustValue),
    _adjustTime(adjustTime),
    _proofingScreen(proofingScreen)
{}

void MenuActions::proofNowAction() {
    DEBUG_PRINTLN("MenuActions: proofNowAction called");
    _proofingScreen->setNextScreen(_screensManager->getActiveScreen());
    _screensManager->setActiveScreen(_proofingScreen);
    _proofingScreen->begin("9:34", 0, true, false);
}

void MenuActions::proofInAction() {
    DEBUG_PRINTLN("MenuActions: proofInAction called");
    _adjustTime->setNextScreen(_screensManager->getActiveScreen()); // TODO go to screen cooling
    _screensManager->setActiveScreen(_adjustTime);
    _adjustTime->begin("Pousser dans...");
}

void MenuActions::proofAtAction() {
    DEBUG_PRINTLN("MenuActions: proofAtAction called");
    _adjustTime->setNextScreen(_screensManager->getActiveScreen()); // TODO go to screen cooling
    _screensManager->setActiveScreen(_adjustTime);
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
    _adjustValue->setNextScreen(_screensManager->getActiveScreen());
    _screensManager->setActiveScreen(_adjustValue); // Set the active screen first
    _adjustValue->begin("Temp\xC3\xA9rature\n" "de chauffe vis\xC3\xA9" "e", "/hot/target_temp.txt");
}

void MenuActions::adjustHotLowerLimit() {
    DEBUG_PRINTLN("MenuActions: adjustHotLowerLimit called");
    _adjustValue->setNextScreen(_screensManager->getActiveScreen());
    _screensManager->setActiveScreen(_adjustValue);
    _adjustValue->begin("Limite basse\n" "de chauffe", "/hot/lower_limit.txt");
}

void MenuActions::adjustHotHigherLimit() {
    DEBUG_PRINTLN("MenuActions: adjustHotHigherLimit called");
    _adjustValue->setNextScreen(_screensManager->getActiveScreen());
    _screensManager->setActiveScreen(_adjustValue);
    _adjustValue->begin("Limite haute\n" "de chauffe", "/hot/higher_limit.txt");
}

void MenuActions::adjustColdTargetTemp() {
    DEBUG_PRINTLN("MenuActions: adjustColdTargetTemp called");
    _adjustValue->setNextScreen(_screensManager->getActiveScreen());
    _screensManager->setActiveScreen(_adjustValue);
    _adjustValue->begin("Temp\xC3\xA9rature\n" "de froid vis\xC3\xA9" "e", "/cold/target_temp.txt");
}

void MenuActions::adjustColdLowerLimit() {
    DEBUG_PRINTLN("MenuActions: adjustColdLowerLimit called");
    _adjustValue->setNextScreen(_screensManager->getActiveScreen());
    _screensManager->setActiveScreen(_adjustValue);
    _adjustValue->begin("Limite basse\n" "de froid", "/cold/lower_limit.txt");
}

void MenuActions::adjustColdHigherLimit() {
    DEBUG_PRINTLN("MenuActions: adjustColdHigherLimit called");
    _adjustValue->setNextScreen(_screensManager->getActiveScreen());
    _screensManager->setActiveScreen(_adjustValue);
    _adjustValue->begin("Limite haute\n" "de froid", "/cold/higher_limit.txt");
}

void MenuActions::resetWiFiAndReboot() {
    // Reset Wi-Fi credentials
    // WiFiManager wifiManager;
    // wifiManager.resetSettings();

    // // Notify the user
    // _menu->_display->clearBuffer();
    // _menu->_display->setFont(u8g2_font_t0_11_tf);
    // _menu->setCurrentTitle("Reset du Wi-Fi\n" "et red\xC3\xA9marrage");
    // _menu->drawTitle(20);
    // _menu->_display->sendBuffer();

    // Delay to allow the message to be displayed
    delay(2000);

    // // Reboot the device
    // ESP.restart();
}

void MenuActions::adjustTimezone() {
    // _menu->_display->clear();
    // _menu->_display->drawUTF8(10, 20, "Adjusting the timezone...");
    // _menu->_display->sendBuffer();
    delay(1000);
}