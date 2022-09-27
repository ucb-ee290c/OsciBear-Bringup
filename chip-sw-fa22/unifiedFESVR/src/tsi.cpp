#include "tsi.h"

tsi::tsi(uint8_t z, uint8_t o, uint8_t a, uint8_t w) {
    // Current program is hardcoded for the following parameters
    tsi::z = 4;
    tsi::o = 4;
    tsi::a = 32;
    tsi::w = 8;
}

/* To understand why, see https://github.com/ucberkeley-ee290c/fa22/tree/main/labs/lab2-tsi-flow */
uint64_t tsi::length() {
    return 3 + 3 + 3 + tsi::z + tsi::o + tsi::a + 9*((int)tsi::w) + 1 + 1;
}

size_t tsi::bufferLength() {
    // Hardcoded for a set of parameter for now.
    return (size_t)128/8;
}

/* Serializes message given paremeters. */
int tsi::serialize(uint64_t *buffer) {
    // Hardcoded for a set of parameter for now.
    buffer[0] = 0u;
    buffer[1] = 0u;
    // Reminder: signal orders are reversed, but the actual bits aren't...
    // Todo
}

/* Deserialize message and return parameter. */
int tsi::deserialize(uint64_t *buffer) {
    // Todo
}