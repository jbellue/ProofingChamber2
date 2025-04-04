#include "MenuItems.h"
#include <functional>

// Declare the global instance of MenuActions
extern MenuActions menuActions;

Menu::MenuItem mainMenu[] = {
    {"Mettre en pousse", iconProof,    nullptr,      std::bind(&MenuActions::proofNowAction, &menuActions)},
    {"Mettre en froid",  iconCool,     coolMenu,     nullptr},
    {"R\xC3\xA9glages",  iconSettings, settingsMenu, nullptr},
    {nullptr,            nullptr,      nullptr,      nullptr} // End of menu
};

Menu::MenuItem coolMenu[] = {
    {"Pousser \xC3\xA0...", iconClock,     nullptr,  std::bind(&MenuActions::proofAtAction, &menuActions)},
    {"Pousser dans...",     iconHourglass, nullptr,  std::bind(&MenuActions::proofInAction, &menuActions)},
    {"Retour",              iconBack,      mainMenu, nullptr},
    {nullptr,               nullptr,       nullptr,  nullptr} // End of menu
};

Menu::MenuItem settingsMenu[] = {
    {"Chaud",          iconHotSettings,  hotMenu,          nullptr},
    {"Froid",          iconColdSettings, coldMenu,         nullptr},
    {"Avanc\xC3\xA9s", iconSettings,     moreSettingsMenu, nullptr},
    {"Retour",         iconBack,         mainMenu,         nullptr},
    {nullptr,          nullptr,          nullptr,          nullptr} // End of menu
};

Menu::MenuItem moreSettingsMenu[] = {
    {"Reset du WiFi",     iconWiFi,  nullptr,      std::bind(&MenuActions::resetWiFiAndReboot, &menuActions)},
    {"Fuseau horaire",    iconClock, nullptr,      std::bind(&MenuActions::adjustTimezone, &menuActions)},
    {"Red\xC3\xA9marrer", iconReset, nullptr,      std::bind(&MenuActions::reboot, &menuActions)},
    {"Retour",            iconBack,  settingsMenu, nullptr},
    {nullptr,             nullptr,   nullptr,      nullptr} // End of menu
};

Menu::MenuItem hotMenu[] = {
    {"Temp\xC3\xA9rature vis\xC3\xA9" "e", iconTarget,       nullptr,      std::bind(&MenuActions::adjustHotTargetTemp, &menuActions)},
    {"Limite basse",                       iconColdSettings, nullptr,      std::bind(&MenuActions::adjustHotLowerLimit, &menuActions)},
    {"Limite haute",                       iconHotSettings,  nullptr,      std::bind(&MenuActions::adjustHotHigherLimit, &menuActions)},
    {"Retour",                             iconBack,         settingsMenu, nullptr},
    {nullptr,                              nullptr,          nullptr,      nullptr} // End of menu
};

Menu::MenuItem coldMenu[] = {
    {"Temp\xC3\xA9rature vis\xC3\xA9" "e", iconTarget,       nullptr,      std::bind(&MenuActions::adjustColdTargetTemp, &menuActions)},
    {"Limite basse",                       iconColdSettings, nullptr,      std::bind(&MenuActions::adjustColdLowerLimit, &menuActions)},
    {"Limite haute",                       iconHotSettings,  nullptr,      std::bind(&MenuActions::adjustColdHigherLimit, &menuActions)},
    {"Retour",                             iconBack,         settingsMenu, nullptr},
    {nullptr,                              nullptr,          nullptr,      nullptr} // End of menu
};
