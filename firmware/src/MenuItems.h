#pragma once

#include "./screens/Menu.h"
#include "icons.h"
#include "MenuActions.h"

// Forward declaration for AppContext
struct AppContext;

// Initialize dynamic timezone menus (call once at startup)
void initializeAllMenus(AppContext* ctx = nullptr);

extern Menu::MenuItem mainMenu[];
extern Menu::MenuItem delayedProofMenu[];
extern Menu::MenuItem settingsMenu[];
extern Menu::MenuItem moreSettingsMenu[];
extern Menu::MenuItem hotMenu[];
extern Menu::MenuItem coldMenu[];
extern Menu::MenuItem* timezoneMenu;

// Refresh icons to reflect the saved timezone selection without rebuilding menus
void refreshTimezoneSelectionIcons(AppContext* ctx = nullptr);
