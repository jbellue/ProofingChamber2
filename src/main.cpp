#include "Menu.h"
#include "Display.h"
#include "Storage.h"
#include <ESP32Encoder.h>

// Encoder setup
#define ENCODER_CLK 2
#define ENCODER_DT  3
#define ENCODER_SW  4
ESP32Encoder encoder;

// Global objects
Display display;
Storage storage;
Menu menu(display, storage, encoder, ENCODER_SW);

void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Initialize display
    display.begin();

    // Attach the encoder to GPIO pins
    encoder.attachSingleEdge(ENCODER_CLK, ENCODER_DT); // Use single-edge mode for better performance

    // Set the starting count (optional)
    encoder.setCount(0);

    // Initialize menu
    menu.begin();
}

void loop() {
    // Update menu
    menu.update();
}