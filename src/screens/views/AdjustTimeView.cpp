#include "AdjustTimeView.h"

void AdjustTimeView::drawTime(const SimpleTime& time, uint8_t valueY) {
    _display->setFont(u8g2_font_ncenB18_tf);
    char timeBuffer[6];
    sprintf(timeBuffer, "%02d:%02d", time.hours, time.minutes);
    
    // Cache width and metrics to avoid repeated calls
    const uint8_t ascent = _display->getAscent();
    const uint8_t timeWidth = _display->getStrWidth("00:00");
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2;
    const uint8_t timeY = valueY + 2;
    const uint8_t clearWidth = _display->getWidth();
    const uint8_t clearHeight = ascent;
    
    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY - ascent, clearWidth, clearHeight);
    _display->setDrawColor(1);
    _display->drawStr(timeX, timeY, timeBuffer);
    if(time.days > 0) {
        _display->setFont(u8g2_font_ncenB12_tr);
        sprintf(timeBuffer, "+%dj", time.days);
        _display->drawStr(timeX + timeWidth + 2, timeY - 4, timeBuffer);
    }
}

void AdjustTimeView::drawHighlight(uint8_t selectedItem, uint8_t valueY) {
    _display->setFont(u8g2_font_ncenB18_tf);
    
    // Cache width calculations - same font as drawTime
    const uint8_t timeWidth = _display->getStrWidth("00:00");
    const uint8_t colonWidth = _display->getStrWidth("00:");
    const uint8_t digitPairWidth = _display->getStrWidth("00");
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2;
    const uint8_t timeY = valueY + 2;
    
    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY + 2, timeWidth, 2);
    if (selectedItem == 0) {
        _display->setDrawColor(1);
        _display->drawHLine(timeX, timeY + 2, digitPairWidth);
    } else if (selectedItem == 1) {
        _display->setDrawColor(1);
        _display->drawHLine(timeX + colonWidth, timeY + 2, digitPairWidth);
    }
}

void AdjustTimeView::drawButtons(const int8_t selectedButton) {
    const char* buttons[] = {"Démarrer", "Annuler"};
    IBaseView::drawButtons(buttons, 2, selectedButton);
}

uint8_t AdjustTimeView::start(const char* title, const SimpleTime& time, uint8_t selectedItem, int8_t selectedButton) {
    clear();
    uint8_t titleHeight = _display->drawTitle(title);
    drawHighlight(selectedItem, titleHeight);
    drawTime(time, titleHeight);
    drawButtons(selectedButton);
    sendBuffer();
    return titleHeight;
}
