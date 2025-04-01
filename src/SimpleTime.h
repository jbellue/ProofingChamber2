#pragma once

#include <stdint.h>

class SimpleTime {
public:
    SimpleTime(int8_t d = 0, int8_t h = 0, int8_t m = 0) 
        : days(d), hours(h), minutes(m) {}

    bool isGreaterThanOrEqual(const SimpleTime& other) const;

    void incrementHours();

    void incrementMinutes();

    void decrementHours();

    void decrementMinutes();

    int8_t days;
    int8_t hours;
    int8_t minutes;
};