
// File:    debug_output.c
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of debug output functions into logfile

#ifdef WITH_DEBUG_OUTPUT

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "debug_output.h"

static FILE *debug_file;
static uint32_t debug_time[DEBUG_TIME_MAX][2] = {{0,0}};



// Initialize debug output (open file)
void debug_init()
{
    // Open debug file
    debug_file = fopen("debug_output.log", "w");
    if(debug_file != NULL)
    {
        // Set the file stream for debugging
        setvbuf(debug_file, NULL, _IOLBF, 0);
    }
}

// Print debug output (to file)
void debug_printf(const char * format, ...)
{
    if(debug_file != NULL)
    {
        va_list args;
        va_start(args, format);
        vfprintf(debug_file, format, args);
        va_end(args);
    }
}

// Start debug time measurement
void debug_time_start(uint8_t num)
{
    if(num < DEBUG_TIME_MAX)
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        debug_time[num][0] = ts.tv_nsec / 1000;
    }
}

// Stop debug time measurement
void debug_time_stop(uint8_t num)
{
    if(num < DEBUG_TIME_MAX)
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        debug_time[num][1] = ts.tv_nsec / 1000;
    }
}

// Get debug time measurement
uint32_t debug_time_get(uint8_t num)
{
    if(num < DEBUG_TIME_MAX)
    {
        return ((debug_time[num][1] - debug_time[num][0] + 1000000) % 1000000);
    }
    else
    {
        return 0;
    }
}



#endif// WITH_DEBUG_OUTPUT
