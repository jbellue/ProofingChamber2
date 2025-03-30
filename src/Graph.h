#pragma once
#include <Arduino.h>
#include <U8g2lib.h>

// Maximum dimensions for the entire application
#define MAX_GRAPH_WIDTH  128
#define MAX_GRAPH_HEIGHT 32

class Graph {
public:
    // Configure graph for current use
    void configure(const int width, const int height, const float minValue, const float maxValue, bool drawAxes = false);

    // Add a value to be averaged
    void addValueToAverage(const float value);

    // Calculate average and add it as a point
    void commitAverage();

    // Add a data point directly (no averaging)
    void addPoint(const float value);

    // Draw the graph
    void draw(U8G2* display, const uint8_t xPos, const uint8_t yPos);

    // Clear the graph
    void clear();

private:
    static uint8_t _buffer[(MAX_GRAPH_WIDTH * MAX_GRAPH_HEIGHT) / 8]; // Static buffer

    uint8_t _width, _height;        // Current dimensions
    bool _drawAxes;                 // Draw axis or not
    float _minValue, _maxValue;     // Value range
    uint8_t _currentIndex;          // Current position in circular buffer
    float _values[MAX_GRAPH_WIDTH]; // Circular buffer for values

    // Averaging support
    float _sumForAverage;
    uint16_t _countForAverage;
};