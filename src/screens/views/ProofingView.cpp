#include "ProofingView.h"
#include "../../DebugUtils.h"
#include "../../icons.h"

void ProofingView::drawTime(const char* timeBuffer) {
    setFont(u8g2_font_ncenB18_tf);

    const uint8_t timeWidth = _display->getStrWidth(timeBuffer);
    const uint8_t timeX = (_display->getDisplayWidth() - timeWidth) / 2;
    const uint8_t timeY = 36;
    const uint8_t fontHeight = _display->getAscent() - _display->getDescent();

    _display->setDrawColor(0);
    _display->drawBox(timeX, timeY - _display->getAscent(), timeWidth, fontHeight);

    _display->setDrawColor(1);
    _display->drawUTF8(timeX, timeY, timeBuffer);
}

void ProofingView::drawTemperature(const char* tempBuffer) {
    setFont(u8g2_font_t0_11_tf);
    const uint8_t tempWidth = _display->getUTF8Width("99.9°");
    const uint8_t tempHeight = _display->getAscent() - _display->getDescent();
    const uint8_t tempX = _display->getDisplayWidth() - tempWidth;
    const uint8_t tempY = 44;

    _display->setDrawColor(0);
    _display->drawBox(tempX, tempY - _display->getAscent(), tempWidth, tempHeight);
    _display->setDrawColor(1);

    _display->drawUTF8(tempX, tempY, tempBuffer);
}

void ProofingView::drawIcons(bool on) {
    const uint8_t proofIconSize = 10;
    const uint8_t iconsX = _display->getDisplayWidth() - proofIconSize - 2;
    const uint8_t iconY = 35;

    if (on) {
        _display->drawXBMP(iconsX, iconY, proofIconSize, proofIconSize, iconProof);
    } else {
        _display->setDrawColor(0);
        _display->drawBox(iconsX, iconY, proofIconSize, proofIconSize);
        _display->setDrawColor(1);
    }
}

void ProofingView::drawButtons() {
    drawButton("Annuler", true);
}

void ProofingView::drawGraph(Graph& graph) {
    _display->setDrawColor(1);
    graph.draw(_display->getDisplay(), _display->getWidth() - 30, 48);
}
