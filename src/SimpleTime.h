#pragma once

#include <stdint.h>

class SimpleTime {
public:
    SimpleTime(int8_t d = 0, int8_t h = 0, int8_t m = 0) 
        : days(d), hours(h), minutes(m) {}

    int8_t days;
    int8_t hours;
    int8_t minutes;

    void incrementHours();
    void incrementMinutes();
    void decrementHours();
    void decrementMinutes();

    // Comparison operators
    bool operator==(const SimpleTime& other) const {
        return days == other.days && hours == other.hours && minutes == other.minutes;
    }

    bool operator!=(const SimpleTime& other) const {
        return !(*this == other);
    }

    bool operator>(const SimpleTime& other) const {
        if (days != other.days) return days > other.days;
        if (hours != other.hours) return hours > other.hours;
        return minutes > other.minutes;
    }

    bool operator<(const SimpleTime& other) const {
        return other > *this;
    }

    bool operator>=(const SimpleTime& other) const {
        return !(*this < other);
    }

    bool operator<=(const SimpleTime& other) const {
        return !(*this > other);
    }
};