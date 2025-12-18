#include "MenuItems.h"
#include "Timezones.h"

// MenuActions instance will be set at runtime; we use a pointer
extern MenuActions* menuActions;
// Define the exported pointer for the dynamically allocated timezone menu
Menu::MenuItem* timezoneMenu = nullptr;

// Dynamic timezone menu generation
namespace {
    // Allocate space for dynamically generated timezone menus
    static Menu::MenuItem** timezoneSubmenus = nullptr;
    static Menu::MenuItem* timezoneMenuAllocated = nullptr;
    static bool timezoneMenusInitialized = false;

    void initializeTimezoneMenus() {
        if (timezoneMenusInitialized) return;

        // Allocate array of pointers to timezone menus (one per continent)
        timezoneSubmenus = new Menu::MenuItem*[timezones::CONTINENT_COUNT];

        // Allocate and populate the top-level timezone (continent) menu dynamically
        // Size: continents + 1 for "Retour" + 1 for terminator
        timezoneMenuAllocated = new Menu::MenuItem[timezones::CONTINENT_COUNT + 2];
        for (int c = 0; c < timezones::CONTINENT_COUNT; c++) {
            timezoneMenuAllocated[c].name = timezones::CONTINENTS[c].name;
            timezoneMenuAllocated[c].icon = nullptr;
            timezoneMenuAllocated[c].subMenu = nullptr; // set after submenus are created
            timezoneMenuAllocated[c].action = nullptr;
        }
        // Add "Retour" at the end
        timezoneMenuAllocated[timezones::CONTINENT_COUNT].name = "Retour";
        timezoneMenuAllocated[timezones::CONTINENT_COUNT].icon = iconBack;
        timezoneMenuAllocated[timezones::CONTINENT_COUNT].subMenu = moreSettingsMenu;
        timezoneMenuAllocated[timezones::CONTINENT_COUNT].action = nullptr;
        // Null terminator
        timezoneMenuAllocated[timezones::CONTINENT_COUNT + 1] = {nullptr, nullptr, nullptr, nullptr};

        // Build submenu for each continent
        for (int c = 0; c < timezones::CONTINENT_COUNT; c++) {
            const timezones::Continent& continent = timezones::CONTINENTS[c];
            
            // Allocate menu items for this continent (timezones + 1 for "Retour" + 1 for null terminator)
            timezoneSubmenus[c] = new Menu::MenuItem[continent.count + 2];
            
            // Populate menu items from timezone data
            for (int i = 0; i < continent.count; i++) {
                timezoneSubmenus[c][i].name = continent.timezones[i].name;
                timezoneSubmenus[c][i].icon = nullptr;
                timezoneSubmenus[c][i].subMenu = nullptr;
                // All timezone items use the same generic handler that uses timezone data
                timezoneSubmenus[c][i].action = &MenuActions::selectTimezoneByData;
            }
            
            // Add "Retour" item to go back to parent menu
            timezoneSubmenus[c][continent.count].name = "Retour";
            timezoneSubmenus[c][continent.count].icon = iconBack;
            timezoneSubmenus[c][continent.count].subMenu = timezoneMenuAllocated;
            timezoneSubmenus[c][continent.count].action = nullptr;
            
            // Add null terminator
            timezoneSubmenus[c][continent.count + 1] = {nullptr, nullptr, nullptr, nullptr};
        }

        timezoneMenusInitialized = true;
    }
}

// Call this from MenuItems initialization
void initializeAllMenus() {
    initializeTimezoneMenus();
    
    // Set up timezone submenu pointers after initialization
    for (int c = 0; c < timezones::CONTINENT_COUNT; c++) {
        timezoneMenuAllocated[c].subMenu = timezoneSubmenus[c];
    }

    // Expose the allocated menu via the extern pointer
    timezoneMenu = timezoneMenuAllocated;

    // Ensure the "Fuseau horaire" item points to the initialized timezone menu
    // Item index 2 in moreSettingsMenu corresponds to "Fuseau horaire"
    moreSettingsMenu[2].subMenu = timezoneMenu;
}

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
    {"Donn\xC3\xA9" "es", iconCool,  nullptr,       &MenuActions::showDataDisplay},
    {"Reset du WiFi",     iconWiFi,  nullptr,       &MenuActions::resetWiFiAndReboot},
    {"Fuseau horaire",    iconClock, timezoneMenu,  nullptr},
    {"Red\xC3\xA9marrer", iconReset, nullptr,       &MenuActions::reboot},
    {"Retour",            iconBack,  settingsMenu,  nullptr},
    {nullptr,             nullptr,   nullptr,       nullptr} // End of menu
};

Menu::MenuItem hotMenu[] = {
    {"Limite basse",                       iconColdSettings, nullptr,      &MenuActions::adjustHotLowerLimit},
    {"Limite haute",                       iconHotSettings,  nullptr,      &MenuActions::adjustHotHigherLimit},
    {"Retour",                             iconBack,         settingsMenu, nullptr},
    {nullptr,                              nullptr,          nullptr,      nullptr} // End of menu
};

Menu::MenuItem coldMenu[] = {
    {"Limite basse",                       iconColdSettings, nullptr,      &MenuActions::adjustColdLowerLimit},
    {"Limite haute",                       iconHotSettings,  nullptr,      &MenuActions::adjustColdHigherLimit},
    {"Retour",                             iconBack,         settingsMenu, nullptr},
    {nullptr,                              nullptr,          nullptr,      nullptr} // End of menu
};


