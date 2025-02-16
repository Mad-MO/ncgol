
// File:    patterns.h
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the patterns functions for the Game of Life

#ifndef __PATTERNS_H
#define __PATTERNS_H

#include <stdint.h>
#include "grid.h"

typedef enum
{
    PATTERN_BEACON,
    PATTERN_BLINKER,
    PATTERN_BLOCK,
    PATTERN_CONWAY,
    PATTERN_GLIDER,
    // ------------
    PATTERN_MAX
} pattern_t;



void init_pattern_to_grid(pattern_t pattern, grid_t *grid, uint16_t x_pos, uint16_t y_pos);


#endif // __PATTERNS_H
