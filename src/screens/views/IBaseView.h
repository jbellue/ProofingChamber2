#pragma once

#include "../../DisplayManager.h"

class IBaseView {
public:
    explicit IBaseView(DisplayManager* display) : _display(display) {}
    virtual ~IBaseView() = default;

    // Common interface for all views
    virtual void clear() { if (_display) _display->clear(); }
    virtual void sendBuffer() { if (_display) _display->sendBuffer(); }

protected:
    DisplayManager* _display;
};
