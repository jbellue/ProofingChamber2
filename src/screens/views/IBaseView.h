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
    virtual void drawButton(const char* text, bool selected = true) { if (_display) _display->drawButton(text, selected); }
    virtual void drawButtons(const char* leftText, const char* rightText, int8_t selectedButton = -1) { if (_display) _display->drawButtons(leftText, rightText, selectedButton); }
    virtual void setFont(const uint8_t* font) { if (_display) _display->setFont(font); }

protected:
    DisplayManager* _display;
};
