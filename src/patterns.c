
// File:    patterns.c
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the patterns functions for the Game of Life
//
// Rules:   https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

#include <stdint.h>
#include "patterns.h"
#include "patterns_data.h"
#include "grid.h"



void init_pattern_to_grid(pattern_t pattern, grid_t *grid, uint16_t x_pos, uint16_t y_pos)
{
    uint16_t x;
    uint16_t y;
    pattern_desc_t *pattern_desc = pattern_list[pattern];

    // TODO: Check boundaries

    for(x=0; x<pattern_desc->width; x++)
    {
        for(y=0; y<pattern_desc->height; y++)
        {
            grid[x+x_pos][y+y_pos] = pattern_desc->pattern[x + y*pattern_desc->width];
        }
    }
}
