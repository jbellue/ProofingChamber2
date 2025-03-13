#include "Menu.h"
#include <Arduino.h>

// Global objects
Menu menu;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Initialize menu
    menu.begin();
}

void loop() {
    menu.update();
}
