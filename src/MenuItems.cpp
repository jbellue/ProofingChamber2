#include "MenuItems.h"
#include "Timezones.h"
#include "AppContext.h"
#include "services/IStorage.h"
#include <cstring>

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
    
    // Helper function to find the current timezone based on saved posix string
    // Returns the global timezone index
    // Returns DEFAULT_TIMEZONE_INDEX if not found
    int findCurrentTimezone(const char* posixString) {
        if (!posixString || posixString[0] == '\0') {
            // Return default timezone (Paris) if no timezone is saved
            return timezones::DEFAULT_TIMEZONE_INDEX;
        }
        
        // Use the helper function from Timezones.h
        return timezones::findTimezoneIndex(posixString);
    }
    
    // Helper to get continent index for a timezone
    int getContinentIndexForTimezone(int timezoneIndex) {
        if (timezoneIndex < 0 || timezoneIndex >= timezones::TIMEZONE_COUNT) {
            return 0;
        }
        
        const char* tzContinent = timezones::TIMEZONES[timezoneIndex].continent;
        int continentCount = timezones::getContinentCount();
        
        for (int c = 0; c < continentCount; c++) {
            const char* continentName = timezones::getContinentName(c);
            if (strcmp(continentName, tzContinent) == 0) {
                return c;
            }
        }
        return 0;
    }
    
    // Helper to get local timezone index within a continent
    int getLocalTimezoneIndex(int globalTimezoneIndex) {
        if (globalTimezoneIndex < 0 || globalTimezoneIndex >= timezones::TIMEZONE_COUNT) {
            return 0;
        }
        
        const char* targetContinent = timezones::TIMEZONES[globalTimezoneIndex].continent;
        int localIndex = 0;
        
        for (int i = 0; i < globalTimezoneIndex; i++) {
            if (strcmp(timezones::TIMEZONES[i].continent, targetContinent) == 0) {
                localIndex++;
            }
        }
        return localIndex;
    }

    void initializeTimezoneMenus(AppContext* ctx) {
        if (timezoneMenusInitialized) return;
        
        // Try to read current timezone from storage
        char currentTimezone[64] = "";
        if (ctx && ctx->storage) {
            ctx->storage->readString("/timezone.txt", currentTimezone, sizeof(currentTimezone), "");
        }
        
        // Find which timezone is currently selected
        int currentTimezoneIndex = findCurrentTimezone(currentTimezone);
        int currentContinentIndex = getContinentIndexForTimezone(currentTimezoneIndex);
        int currentLocalIndex = getLocalTimezoneIndex(currentTimezoneIndex);

        // Get continent count
        int continentCount = timezones::getContinentCount();

        // Allocate array of pointers to timezone menus (one per continent)
        timezoneSubmenus = new Menu::MenuItem*[continentCount];

        // Allocate and populate the top-level timezone (continent) menu dynamically
        // Size: continents + 1 for "Retour" + 1 for terminator
        timezoneMenuAllocated = new Menu::MenuItem[continentCount + 2];
        for (int c = 0; c < continentCount; c++) {
            const char* continentName = timezones::getContinentName(c);
            timezoneMenuAllocated[c].name = continentName;
            // Show icon in front of currently selected continent
            timezoneMenuAllocated[c].icon = (c == currentContinentIndex) ? iconCheck : nullptr;
            timezoneMenuAllocated[c].subMenu = nullptr; // set after submenus are created
            timezoneMenuAllocated[c].action = nullptr;
        }
        // Add "Retour" at the end
        timezoneMenuAllocated[continentCount].name = "Retour";
        timezoneMenuAllocated[continentCount].icon = iconBack;
        timezoneMenuAllocated[continentCount].subMenu = moreSettingsMenu;
        timezoneMenuAllocated[continentCount].action = nullptr;
        // Null terminator
        timezoneMenuAllocated[continentCount + 1] = {nullptr, nullptr, nullptr, nullptr};

        // Build submenu for each continent
        for (int c = 0; c < continentCount; c++) {
            const char* continentName = timezones::getContinentName(c);
            int tzCount = timezones::getTimezoneCount(continentName);
            
            // Allocate menu items for this continent (timezones + 1 for "Retour" + 1 for null terminator)
            timezoneSubmenus[c] = new Menu::MenuItem[tzCount + 2];
            
            // Populate menu items from timezone data
            for (int i = 0; i < tzCount; i++) {
                const timezones::Timezone* tz = timezones::getTimezone(continentName, i);
                if (!tz) continue;
                
                // Check if this is the currently selected timezone
                bool isCurrent = (c == currentContinentIndex && i == currentLocalIndex);
                timezoneSubmenus[c][i].name = tz->name;
                timezoneSubmenus[c][i].icon = isCurrent ? iconCheck : nullptr;
                timezoneSubmenus[c][i].subMenu = nullptr;
                // All timezone items use the same generic handler that uses timezone data
                timezoneSubmenus[c][i].action = &MenuActions::selectTimezoneByData;
            }
            
            // Add "Retour" item to go back to parent menu
            timezoneSubmenus[c][tzCount].name = "Retour";
            timezoneSubmenus[c][tzCount].icon = iconBack;
            timezoneSubmenus[c][tzCount].subMenu = timezoneMenuAllocated;
            timezoneSubmenus[c][tzCount].action = nullptr;
            
            // Add null terminator
            timezoneSubmenus[c][tzCount + 1] = {nullptr, nullptr, nullptr, nullptr};
        }

        timezoneMenusInitialized = true;
    }
}

// Call this from MenuItems initialization
void initializeAllMenus(AppContext* ctx) {
    initializeTimezoneMenus(ctx);
    
    int continentCount = timezones::getContinentCount();
    
    // Set up timezone submenu pointers after initialization
    for (int c = 0; c < continentCount; c++) {
        timezoneMenuAllocated[c].subMenu = timezoneSubmenus[c];
    }

    // Expose the allocated menu via the extern pointer
    timezoneMenu = timezoneMenuAllocated;

    // Ensure the "Fuseau horaire" item points to the initialized timezone menu
    // Item index 2 in moreSettingsMenu corresponds to "Fuseau horaire"
    moreSettingsMenu[2].subMenu = timezoneMenu;
}

// Update the check icons to match current saved timezone selection
void refreshTimezoneSelectionIcons(AppContext* ctx) {
    // Ensure menus exist
    if (!timezoneMenusInitialized) {
        initializeTimezoneMenus(ctx);
    }
    // Read current timezone
    char currentTimezone[64] = "";
    if (ctx && ctx->storage) {
        ctx->storage->readString("/timezone.txt", currentTimezone, sizeof(currentTimezone), "");
    }
    // Find indices
    int currentTimezoneIndex = findCurrentTimezone(currentTimezone);
    int currentContinentIndex = getContinentIndexForTimezone(currentTimezoneIndex);
    int currentLocalIndex = getLocalTimezoneIndex(currentTimezoneIndex);
    
    int continentCount = timezones::getContinentCount();
    
    // Update continent icons
    for (int c = 0; c < continentCount; c++) {
        if (timezoneMenuAllocated) {
            timezoneMenuAllocated[c].icon = (c == currentContinentIndex) ? iconCheck : nullptr;
        }
        // Update timezone icons per continent
        const char* continentName = timezones::getContinentName(c);
        int tzCount = timezones::getTimezoneCount(continentName);
        if (timezoneSubmenus) {
            for (int i = 0; i < tzCount; i++) {
                timezoneSubmenus[c][i].icon = (c == currentContinentIndex && i == currentLocalIndex) ? iconCheck : nullptr;
            }
        }
    }
}

Menu::MenuItem mainMenu[] = {
    {"Pousse imm\xC3\xA9" "diate",       iconProof,    nullptr,          &MenuActions::proofNowAction},
    {"Pousse diff\xC3\xA9r\xC3\xA9" "e", iconCool,     delayedProofMenu, nullptr},
    {"R\xC3\xA9glages",                  iconSettings, settingsMenu,     nullptr},
    {nullptr,                            nullptr,      nullptr,          nullptr} // End of menu
};

Menu::MenuItem delayedProofMenu[] = {
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


