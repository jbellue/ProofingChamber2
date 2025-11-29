#include "MenuItems.h"

// MenuActions instance will be set at runtime; we use a pointer
extern MenuActions* menuActions;

Menu::MenuItem mainMenu[] = {
    {"Mettre en pousse", iconProof,    nullptr,      &MenuActions::proofNowAction},
    {"Mettre en froid",  iconCool,     coolMenu,     nullptr},
    {"R\xC3\xA9glages",  iconSettings, settingsMenu, nullptr},
    {nullptr,            nullptr,      nullptr,      nullptr} // End of menu
};

Menu::MenuItem coolMenu[] = {
    {"Pousser \xC3\xA0...", iconClock,     nullptr,  &MenuActions::proofAtAction},
    {"Pousser dans...",     iconHourglass, nullptr,  &MenuActions::proofInAction},
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
    {"Reset du WiFi",     iconWiFi,  nullptr,      &MenuActions::resetWiFiAndReboot},
    {"Fuseau horaire",    iconClock, nullptr,      &MenuActions::adjustTimezone},
    {"Red\xC3\xA9marrer", iconReset, nullptr,      &MenuActions::reboot},
    {"Retour",            iconBack,  settingsMenu, nullptr},
    {nullptr,             nullptr,   nullptr,      nullptr} // End of menu
};

Menu::MenuItem hotMenu[] = {
    {"Temp\xC3\xA9rature vis\xC3\xA9" "e", iconTarget,       nullptr,      &MenuActions::adjustHotTargetTemp},
    {"Limite basse",                       iconColdSettings, nullptr,      &MenuActions::adjustHotLowerLimit},
    {"Limite haute",                       iconHotSettings,  nullptr,      &MenuActions::adjustHotHigherLimit},
    {"Retour",                             iconBack,         settingsMenu, nullptr},
    {nullptr,                              nullptr,          nullptr,      nullptr} // End of menu
};

Menu::MenuItem coldMenu[] = {
    {"Temp\xC3\xA9rature vis\xC3\xA9" "e", iconTarget,       nullptr,      &MenuActions::adjustColdTargetTemp},
    {"Limite basse",                       iconColdSettings, nullptr,      &MenuActions::adjustColdLowerLimit},
    {"Limite haute",                       iconHotSettings,  nullptr,      &MenuActions::adjustColdHigherLimit},
    {"Retour",                             iconBack,         settingsMenu, nullptr},
    {nullptr,                              nullptr,          nullptr,      nullptr} // End of menu
};
