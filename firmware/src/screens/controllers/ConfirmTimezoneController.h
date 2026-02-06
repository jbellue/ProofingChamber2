#pragma once

#include "../BaseController.h"
#include "../../AppContextDecl.h"

// Forward declarations
class IDisplayManager;
class ConfirmTimezoneView;
class IInputManager;

class ConfirmTimezoneController : public BaseController {
public:
    ConfirmTimezoneController(AppContext* ctx);
    bool update(bool forceRedraw = false) override;

    // Set the timezone info to confirm
    void setTimezoneInfo(const char* continent, const char* tzName, const char* posixString, int timezoneIndex);

private:
    ConfirmTimezoneView* _view;
    bool _onCancelButton = true;
    const char* _timezoneDisplayName = nullptr;
    const char* _timezonePosixString = nullptr;
    const char* _timezoneContinentName = nullptr;
    int _timezoneIndex = -1;

    void beginImpl() override;
};
