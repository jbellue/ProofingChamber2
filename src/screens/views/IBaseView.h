#pragma once

#include "../../DisplayManager.h"

class IBaseView {
public:
    explicit IBaseView(DisplayManager* display) : _display(display) {}
    virtual ~IBaseView() = default;

    // Common interface for all views
    virtual void clear() { if (_display) _display->clear(); }
    virtual void sendBuffer() { if (_display) _display->sendBuffer(); }
    virtual void drawTitle(const char* title, uint8_t y = 10) { if (_display) _display->drawTitle(title, y); }
    virtual void drawButtons(const char* buttonTexts[], uint8_t buttonCount, int8_t selectedButton = -1) { if (_display) _display->drawButtons(buttonTexts, buttonCount, selectedButton); }
    virtual void setFont(const uint8_t* font) { if (_display) _display->setFont(font); }
protected:
    DisplayManager* _display;
};
