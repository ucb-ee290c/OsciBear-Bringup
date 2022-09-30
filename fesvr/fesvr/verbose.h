// See LICENSE for license details.

#ifndef __VERBOSE_H
#define __VERBOSE_H

#include <stdio.h>
#include <stdarg.h>
#include <string>

// These are special terminal characters that start a colored section of text
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
// This terminal character stops a colored section of text
#define RESET "\x1B[0m"

enum class verbose_level_t: unsigned int {
    VERBOSE_NONE = 0,
    VERBOSE_HTIF = 1 << 0,
    VERBOSE_MMIO = 1 << 1 
};

extern verbose_level_t GLOBAL_VERBOSITY_LEVEL;

inline verbose_level_t operator|(verbose_level_t a, verbose_level_t b) {
    return static_cast<verbose_level_t>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline verbose_level_t operator&(verbose_level_t a, verbose_level_t b) {
    return static_cast<verbose_level_t>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

void verbose_print(verbose_level_t v_level, std::string format_string, ...);
#endif // __VERBOSE_H
