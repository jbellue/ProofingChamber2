#include "SimpleTime.h"

void SimpleTime::incrementHours() {
    hours++;
    if (hours > 23) {
        hours = 0;
        days++;
    }
}

void SimpleTime::incrementMinutes() {
    minutes++;
    if (minutes > 59) {
        minutes = 0;
        incrementHours();
    }
}

void SimpleTime::decrementHours() {
    if (hours > 0) {
        hours--;
    } else if (days > 0) {
        days--;
        hours = 23;
    }
}

void SimpleTime::decrementMinutes() {
    if (minutes > 0) {
        minutes--;
    } else if (hours > 0 || days > 0) {
        if (hours == 0) {
            days--;
            hours = 23;
        } else {
            hours--;
        }
        minutes = 59;
    }
}