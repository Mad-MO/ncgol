
// File:    patterns.h
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the patterns functions for the Game of Life

#ifndef __PATTERNS_H
#define __PATTERNS_H

#include <stdint.h>
#include "grid.h"

// Has to be with same content as "pattern_list[]" in "patterns_data.h"
typedef enum
{
    PATTERN_CONWAY,
    PATTERN_CONWAY_FULL,
    PATTERN_BLOCK,
    PATTERN_BEEHIVE,
    PATTERN_LOAF,
    PATTERN_BOAT,
    PATTERN_TUB,
    PATTERN_BLINKER,
    PATTERN_TOAD,
    PATTERN_BEACON,
    PATTERN_PULSAR,
    PATTERN_PENTA_DECATHLON,
    PATTERN_GLIDER,
    PATTERN_GLIDER_STOPPER_BELOW,
    PATTERN_GLIDER_STOPPER_ABOVE,
    PATTERN_LWSS,
    PATTERN_MWSS,
    PATTERN_HWSS,
    PATTERN_GOSPER_GLIDERGUN,
    PATTERN_SIMKIN_GLIDERGUN,
    PATTERN_PENTOMINO,
    PATTERN_DIEHARD,
    PATTERN_ACORN,
    PATTERN_BLOCKENGINE1,
    PATTERN_BLOCKENGINE2,
    PATTERN_DOUBLEBLOCKENGINE,
    PATTERN_ILOVE8BIT,
    // ------------
    PATTERN_MAX
} pattern_t;



// Set pattern to grid at position
void patterns_set_to_pos(pattern_t pattern, grid_t *grid, uint16_t x_pos, uint16_t y_pos);

// Set pattern to grid center
void patterns_set_to_center(pattern_t pattern, grid_t *grid);

// Get pattern width
uint16_t patterns_get_width(pattern_t pattern);

// Get pattern height
uint16_t patterns_get_height(pattern_t pattern);



#endif // __PATTERNS_H
