#include "Graph.h"

void Graph::configure(const int w, const int h, const float minValue, const float maxValue, bool drawAxes) {
    _width = min(w, MAX_GRAPH_WIDTH);
    _height = min(h, MAX_GRAPH_HEIGHT);
    _minValue = minValue;
    _maxValue = maxValue;
    _currentIndex = 0;
    _drawAxes = drawAxes;
    
    // Initialize values array with invalid temperature marker
    for (uint8_t i = 0; i < MAX_GRAPH_WIDTH; i++) {
        _values[i] = -274.0f;
    }
}

void Graph::addValueToAverage(const float value) {
    _sumForAverage += value;
    _countForAverage++;
}

void Graph::commitAverage(const float defaultValue) {
    // If no values were added, use the default value
    if (_countForAverage > 0) {
        const float average = _sumForAverage / _countForAverage;
        addPoint(average);
    }
    else {
        addPoint(defaultValue);
    }
    // Reset averaging
    _sumForAverage = 0;
    _countForAverage = 0;
}

void Graph::addPoint(float value) {
    _values[_currentIndex] = value;
    _currentIndex = (_currentIndex + 1) % _width;
}

void Graph::draw(U8G2* display, const uint8_t xPos, uint8_t yPos) {
    // Clear the graph area
    display->setDrawColor(0);
    display->drawBox(xPos, yPos, _width, _height);
    display->setDrawColor(1);
    uint8_t usableHeight = _height;

    // Draw axes
    if (_drawAxes) {
        display->drawFrame(xPos, yPos, _width, _height);
        usableHeight -= 2; // Adjust height for axes
        yPos++; // Offset by 1 to account for top border
    }

    // Draw data points
    for (uint8_t x = 0; x < _width; x++) {
        const uint8_t idx = (_currentIndex + x) % _width;
        const float val = _values[idx];
        if (val < -273.0) continue; // Skip invalid values
        const uint8_t y = yPos + usableHeight - 1 - static_cast<uint8_t>((val - _minValue) / (_maxValue - _minValue) * (usableHeight - 1));

        // Draw point
        display->drawPixel(xPos + x, y);
    }
}

void Graph::clear() {
    // Initialize _values array with invalid temperature marker
    for (uint8_t i = 0; i < MAX_GRAPH_WIDTH; i++) {
        _values[i] = -274.0f;
    }
    _currentIndex = 0;
    _sumForAverage = 0;
    _countForAverage = 0;
}