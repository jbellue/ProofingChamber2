#include <WiFiManager.h>
#include "MenuActions.h"
#include "DebugUtils.h"

// Define static members
Menu* MenuActions::_menu = nullptr;

void MenuActions::proofNowAction() {
    _menu->display->clear();
    _menu->display->drawUTF8(10, 20, "Proofing now...");
    _menu->display->sendBuffer();
    delay(1000);
}

void MenuActions::proofInAction() {
    _menu->startSetTime("Pousser dans...");
}

void MenuActions::proofAtAction() {
    struct tm timeinfo;
    int hour, minute;
    if (!getLocalTime(&timeinfo)) {
        DEBUG_PRINTLN("Failed to obtain time, defaulting to 0:00");
        _menu->startSetTime("Pousser \xC3\xA0...");
    }
    _menu->startSetTime("Pousser \xC3\xA0...", timeinfo.tm_hour, timeinfo.tm_min);
}

void MenuActions::clockAction() {
    _menu->display->clear();
    _menu->display->drawUTF8(10, 20, "Clock Settings...");
    _menu->display->sendBuffer();
    delay(1000);
}

void MenuActions::adjustHotTargetTemp() {
    _menu->startAdjustValue("Temp\xC3\xA9rature\n" "de chauffe vis\xC3\xA9" "e", "/hot/target_temp.txt");
}

void MenuActions::adjustHotLowerLimit() {
    _menu->startAdjustValue("Limite basse\n" "de chauffe", "/hot/lower_limit.txt");
}

void MenuActions::adjustHotHigherLimit() {
    _menu->startAdjustValue("Limite haute\n" "de chauffe", "/hot/higher_limit.txt");
}

void MenuActions::adjustColdTargetTemp() {
    _menu->startAdjustValue("Temp\xC3\xA9rature\n" "de froid vis\xC3\xA9" "e", "/cold/target_temp.txt");
}

void MenuActions::adjustColdLowerLimit() {
    _menu->startAdjustValue("Limite basse\n" "de froid", "/cold/lower_limit.txt");
}

void MenuActions::adjustColdHigherLimit() {
    _menu->startAdjustValue("Limite haute\n" "de froid", "/cold/higher_limit.txt");
}

void MenuActions::resetWiFiAndReboot() {
    // Reset Wi-Fi credentials
    WiFiManager wifiManager;
    wifiManager.resetSettings();

    // Notify the user
    _menu->display->clearBuffer();
    _menu->display->setFont(u8g2_font_t0_11_tf);
    _menu->setCurrentTitle("Reset du Wi-Fi\n" "et red\xC3\xA9marrage");
    _menu->drawTitle(20);
    _menu->display->sendBuffer();

    // Delay to allow the message to be displayed
    delay(2000);

    // Reboot the device
    ESP.restart();
}

void MenuActions::adjustTimezone() {
    _menu->display->clear();
    _menu->display->drawUTF8(10, 20, "Adjusting the timezone...");
    _menu->display->sendBuffer();
    delay(1000);
}