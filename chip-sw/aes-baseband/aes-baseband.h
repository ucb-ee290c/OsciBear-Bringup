#include <stdint.h>
#include <stdio.h>
#include <string.h>

void createPacket(uint8_t* dest, uint8_t* source, int size) {
    // Assumes that dest has enough space

    // Creating header
    dest[0] = 0x1; // Arbitrary
    dest[1] = (uint8_t) size;

    // Copying data to packet
    memcpy(dest + 2, source, size);
}