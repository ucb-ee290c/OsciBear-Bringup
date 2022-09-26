#include <stdio.h>
#include <stdarg.h>
#include <verbose.h>
#include <string>

// The global declaration of GLOBAL_VERBOSITY_LEVEL
verbose_level_t GLOBAL_VERBOSITY_LEVEL = verbose_level_t::VERBOSE_NONE;

void verbose_print(verbose_level_t v_level, std::string format_string, ...) {
    extern verbose_level_t GLOBAL_VERBOSITY_LEVEL;
    if ((GLOBAL_VERBOSITY_LEVEL & v_level) != verbose_level_t::VERBOSE_NONE) {
        va_list args;
        va_start(args, format_string);
        vprintf(("\t" RED "VERBOSE: " RESET + format_string).c_str(), args);
        va_end(args);
    }
}
