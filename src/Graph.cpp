#include "Graph.h"

// Static buffer definition (zero-initialized)
uint8_t Graph::_buffer[(MAX_GRAPH_WIDTH * MAX_GRAPH_HEIGHT) / 8] = {0};

void Graph::configure(const int w, const int h, const float minValue, const float maxValue, bool drawAxes) {
    _width = min(w, MAX_GRAPH_WIDTH);
    _height = min(h, MAX_GRAPH_HEIGHT);
    _minValue = minValue;
    _maxValue = maxValue;
    _currentIndex = 0;
    _drawAxes = drawAxes;
    memset(_values, -274.0, sizeof(_values));
}

void Graph::addValueToAverage(const float value) {
    _sumForAverage += value;
    _countForAverage++;
}

void Graph::commitAverage() {
    if (_countForAverage > 0) {
        const float average = _sumForAverage / _countForAverage;
        addPoint(average);
        // Reset averaging
        _sumForAverage = 0;
        _countForAverage = 0;
    }
}

void Graph::addPoint(float value) {
    _values[_currentIndex] = value;
    _currentIndex = (_currentIndex + 1) % _width;
}

void Graph::draw(U8G2* display, const uint8_t xPos, const uint8_t yPos) {
    // Clear the graph area
    display->setDrawColor(0);
    display->drawBox(xPos, yPos, _width, _height);
    display->setDrawColor(1);

    // Draw axes
    if (_drawAxes) {
        display->drawFrame(xPos, yPos, _width, _height);
    }

    // Draw data points
    for (uint8_t x = 0; x < _width; x++) {
        uint8_t idx = (_currentIndex + x) % _width;
        float val = _values[idx];
        if (val < -273.0) continue; // Skip invalid values
        uint8_t y = yPos + _height - 1 - static_cast<uint8_t>((val - _minValue) / (_maxValue - _minValue) * (_height - 1));

        // Draw point
        display->drawPixel(xPos + x, y);
    }
}

void Graph::clear() {
    memset(_buffer, 0, sizeof(_values));
    memset(_values, 0, sizeof(_values));
    _currentIndex = 0;
    _sumForAverage = 0;
    _countForAverage = 0;
}