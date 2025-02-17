
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



void patterns_set_to_pos(pattern_t pattern, grid_t *grid, uint16_t x_pos, uint16_t y_pos)
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



void patterns_set_to_center(pattern_t pattern, grid_t *grid)
{
    uint16_t x_pos = (grid_get_width()  - pattern_list[pattern]->width)  / 2;
    uint16_t y_pos = (grid_get_height() - pattern_list[pattern]->height) / 2;

    patterns_set_to_pos(pattern, grid, x_pos, y_pos);
}

// Return text string for initpattern
const char * patterns_get_str(pattern_t pattern)
{
    // TODO: Check boundaries
    return pattern_list[pattern]->name;
}
