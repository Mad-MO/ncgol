
// File:    debug_output.h
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of debug output functions into logfile

#ifndef __DEBUG_OUTPUT_H
#define __DEBUG_OUTPUT_H

#include <stdint.h>
#include "config.h"

#if (WITH_DEBUG_OUTPUT)

enum
{
    DEBUG_TIME1 = 0,
    DEBUG_TIME2,
    DEBUG_TIME3,
    DEBUG_TIME_MAX
};



// Initialize debug output (open file)
void debug_init();

// Print debug output (to file)
void debug_printf(const char * format, ...);

// Start debug time measurement
void debug_time_start(uint8_t num);

// Stop debug time measurement
void debug_time_stop(uint8_t num);

// Get debug time measurement
uint32_t debug_time_get(uint8_t num);



#endif // (WITH_DEBUG_OUTPUT)

#endif // __DEBUG_OUTPUT_H
