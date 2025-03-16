#ifndef ICONS_H
#define ICONS_H
#include <stdint.h>

const uint8_t iconProof[] = {
    0x24, 0x01, 0x24, 0x01, 0x92, 0x00, 0x92, 0x00, 0x24, 0x01, 0x24, 0x01, 0x92, 0x00, 0x00, 0x00, 0xff, 0x03, 0xff, 0x03
};

const uint8_t iconCool[] = {
    0x00, 0x00, 0x10, 0x00, 0x82, 0x00, 0x54, 0x00, 0x38, 0x00, 0x6d, 0x01, 0x38, 0x00, 0x54, 0x00, 0x82, 0x00, 0x10, 0x00
};

const uint8_t iconSettings[] = {
    0x30, 0x00, 0xb6, 0x01, 0xfe, 0x01, 0xcc, 0x00, 0x87, 0x03, 0x87, 0x03, 0xcc, 0x00, 0xfe, 0x01, 0xb6, 0x01, 0x30, 0x00
};

const uint8_t iconBack[] = {
    0x00, 0x00, 0x04, 0x00, 0x06, 0x00, 0x3f, 0x00, 0x46, 0x00, 0x84, 0x00, 0x80, 0x00, 0x40, 0x00, 0x38, 0x00, 0x00, 0x00
};

const uint8_t iconHourglass[] = {
    0xfe, 0x03, 0x04, 0x01, 0x04, 0x01, 0x88, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x24, 0x01, 0x74, 0x01, 0xfe, 0x03
};

const uint8_t iconClock[] = {
    0x70, 0x00, 0xa8, 0x00, 0x24, 0x01, 0x22, 0x02, 0x3a, 0x02, 0x02, 0x02, 0x04, 0x01, 0x88, 0x00, 0x70, 0x00, 0x00, 0x00
};

const uint8_t iconHotSettings[] = {
    0x10, 0x00, 0x28, 0x00, 0x28, 0x00, 0x38, 0x00, 0x38, 0x00, 0x38, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x38, 0x00
};

const uint8_t iconColdSettings[] = {
    0x10, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x44, 0x00, 0x44, 0x00, 0x7c, 0x00, 0x38, 0x00,
};

const uint8_t iconTarget[] = {
    0x38, 0x00, 0x7c, 0x00, 0xfe, 0x00, 0xee, 0x00, 0xfe, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x38, 0x00, 0x38, 0x00, 0x10, 0x00
};

const uint8_t iconWiFi[] = {
    0x78, 0x00, 0x86, 0x01, 0x31, 0x02, 0xcc, 0x00, 0x02, 0x01, 0x78, 0x00, 0x84, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00
};

// Define the degree symbol as a custom character
const uint8_t degreeSymbol[] U8X8_PROGMEM = {
    0b00111100,
    0b01100110,
    0b11000011,
    0b11000011,
    0b11000011,
    0b01100110,
    0b00111100,
    0b00000000
};
#endif // ICONS_H