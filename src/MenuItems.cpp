#include "MenuItems.h"
#include <functional>

// MenuActions instance will be set at runtime; we use a pointer
extern MenuActions* menuActions;

Menu::MenuItem mainMenu[] = {
    {"Mettre en pousse", iconProof,    nullptr,      []() { if (menuActions) menuActions->proofNowAction(); }},
    {"Mettre en froid",  iconCool,     coolMenu,     nullptr},
    {"R\xC3\xA9glages",  iconSettings, settingsMenu, nullptr},
    {nullptr,            nullptr,      nullptr,      nullptr} // End of menu
};

Menu::MenuItem coolMenu[] = {
    {"Pousser \xC3\xA0...", iconClock,     nullptr,  []() { if (menuActions) menuActions->proofAtAction(); }},
    {"Pousser dans...",     iconHourglass, nullptr,  []() { if (menuActions) menuActions->proofInAction(); }},
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
    {"Reset du WiFi",     iconWiFi,  nullptr,      []() { if (menuActions) menuActions->resetWiFiAndReboot(); }},
    {"Fuseau horaire",    iconClock, nullptr,      []() { if (menuActions) menuActions->adjustTimezone(); }},
    {"Red\xC3\xA9marrer", iconReset, nullptr,      []() { if (menuActions) menuActions->reboot(); }},
    {"Retour",            iconBack,  settingsMenu, nullptr},
    {nullptr,             nullptr,   nullptr,      nullptr} // End of menu
};

Menu::MenuItem hotMenu[] = {
    {"Temp\xC3\xA9rature vis\xC3\xA9" "e", iconTarget,       nullptr,      []() { if (menuActions) menuActions->adjustHotTargetTemp(); }},
    {"Limite basse",                       iconColdSettings, nullptr,      []() { if (menuActions) menuActions->adjustHotLowerLimit(); }},
    {"Limite haute",                       iconHotSettings,  nullptr,      []() { if (menuActions) menuActions->adjustHotHigherLimit(); }},
    {"Retour",                             iconBack,         settingsMenu, nullptr},
    {nullptr,                              nullptr,          nullptr,      nullptr} // End of menu
};

Menu::MenuItem coldMenu[] = {
    {"Temp\xC3\xA9rature vis\xC3\xA9" "e", iconTarget,       nullptr,      []() { if (menuActions) menuActions->adjustColdTargetTemp(); }},
    {"Limite basse",                       iconColdSettings, nullptr,      []() { if (menuActions) menuActions->adjustColdLowerLimit(); }},
    {"Limite haute",                       iconHotSettings,  nullptr,      []() { if (menuActions) menuActions->adjustColdHigherLimit(); }},
    {"Retour",                             iconBack,         settingsMenu, nullptr},
    {nullptr,                              nullptr,          nullptr,      nullptr} // End of menu
};
