
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
    PATTERN_CONWAY,
    PATTERN_BLOCK,
    PATTERN_BEEHIVE,
    PATTERN_LOAF,
    PATTERN_BOAT,
    PATTERN_TUB,
    PATTERN_BLINKER,
    PATTERN_TOAD,
    PATTERN_BEACON,
    PATTERN_PENTA_DECATHLON,
    PATTERN_GLIDER,
    PATTERN_LWSS,
    PATTERN_MWSS,
    PATTERN_HWSS,
    PATTERN_GLIDERGUN,
    PATTERN_PENTOMINO,
    PATTERN_DIEHARD,
    PATTERN_ACORN,
    PATTERN_BLOCKENGINE1,
    PATTERN_BLOCKENGINE2,
    PATTERN_DOUBLEBLOCKENGINE,
    // ------------
    PATTERN_MAX
} pattern_t;



// Set pattern to grid at position
void set_pattern_to_grid_pos(pattern_t pattern, grid_t *grid, uint16_t x_pos, uint16_t y_pos);

// Set pattern to grid center
void set_pattern_to_grid_center(pattern_t, grid_t *grid);

// Return text string for initpattern
const char * get_pattern_str(pattern_t pattern);


#endif // __PATTERNS_H
