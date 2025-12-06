#include "DataDisplayView.h"

void DataDisplayView::start() {
    clear();
    drawTitle();
    drawButtons();
}

void DataDisplayView::drawTitle() {
    const char* title = "Debug des senseurs";
    IBaseView::drawTitle(title);
}
void DataDisplayView::drawButtons() {
    const char* buttons[] = { "Retour" };
    IBaseView::drawButtons(buttons, 1, 0);
}

void DataDisplayView::reset() {
    _lastTemperature = -273.15;
    _lastMinute = -1;
}

bool DataDisplayView::drawTemperature(float temperatureC) {
    if (abs(_lastTemperature - temperatureC) < 0.1) {
        return false; // No significant change, skip redraw
    }
    _lastTemperature = temperatureC;
    char tempBuffer[7] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1f°C", temperatureC);
    _display->setFont(u8g2_font_t0_11_tf);
    const uint8_t tempWidth = _display->getUTF8Width("99.9°C");
    const uint8_t tempHeight = _display->getAscent() - _display->getDescent();
    const uint8_t tempX = 2;
    const uint8_t tempY = 22;

    _display->setDrawColor(0);
    _display->drawBox(tempX, tempY - _display->getAscent(), tempWidth, tempHeight);
    _display->setDrawColor(1);

    _display->drawUTF8(tempX, tempY, tempBuffer);
    return true;
}

bool DataDisplayView::drawTime(const tm &now) {
    if (_lastMinute == now.tm_min) {
        return false; // No change within the same minute
    }
    char timeBuffer[15] = {0}; // "HH:MM DD/MM/YY"
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d %02d/%02d/%02d",
             now.tm_hour, now.tm_min,
             now.tm_mday, now.tm_mon + 1, now.tm_year % 100);

    _display->setFont(u8g2_font_t0_11_tf);
    const uint8_t timeWidth = _display->getUTF8Width(timeBuffer);
    const uint8_t timeHeight = _display->getAscent() - _display->getDescent();
    const uint8_t timeX = _display->getDisplayWidth() - timeWidth - 2;
    const uint8_t timeY = 22;

    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY - _display->getAscent(), timeWidth, timeHeight);
    _display->setDrawColor(1);

    _display->drawUTF8(timeX, timeY, timeBuffer);
    _lastMinute = now.tm_min;
    return true;
}
