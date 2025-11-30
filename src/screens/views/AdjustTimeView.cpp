#include "AdjustTimeView.h"

void AdjustTimeView::drawTime(const SimpleTime& time, uint8_t valueY) {
    _display->setFont(u8g2_font_ncenB18_tf);
    char timeBuffer[6];
    sprintf(timeBuffer, "%02d:%02d", time.hours, time.minutes);
    const uint8_t timeWidth = _display->getStrWidth("00:00");
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2;
    const uint8_t timeY = valueY + 2;
    const uint8_t clearWidth = _display->getWidth();
    const uint8_t clearHeight = _display->getAscent();
    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY - _display->getAscent(), clearWidth, clearHeight);
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
    const uint8_t timeWidth = _display->getStrWidth("00:00");
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2;
    const uint8_t timeY = valueY + 2;
    const uint8_t lineLength = _display->getStrWidth("00");
    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY + 2, timeWidth, 2);
    if (selectedItem == 0) {
        _display->setDrawColor(1);
        _display->drawHLine(timeX, timeY + 2, lineLength);
    } else if (selectedItem == 1) {
        _display->setDrawColor(1);
        _display->drawHLine(timeX + _display->getStrWidth("00:"), timeY + 2, lineLength);
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
