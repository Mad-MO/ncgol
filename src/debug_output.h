
// File:    debug_output.h
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of debug output functions into logfile

#ifdef WITH_DEBUG_OUTPUT


#ifndef __DEBUG_OUTPUT_H
#define __DEBUG_OUTPUT_H

#include <stdint.h>

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



#endif // __DEBUG_OUTPUT_H
#endif // WITH_DEBUG_OUTPUT
