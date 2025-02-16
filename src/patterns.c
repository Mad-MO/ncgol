
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



void set_pattern_to_grid_pos(pattern_t pattern, grid_t *grid, uint16_t x_pos, uint16_t y_pos)
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



void set_pattern_to_grid_center(pattern_t pattern, grid_t *grid)
{
    uint16_t x_pos = (get_grid_width()  - pattern_list[pattern]->width)  / 2;
    uint16_t y_pos = (get_grid_height() - pattern_list[pattern]->height) / 2;

    set_pattern_to_grid_pos(pattern, grid, x_pos, y_pos);
}

// Return text string for initpattern
const char * get_pattern_str(pattern_t pattern)
{
    // TODO: Check boundaries
    return pattern_list[pattern]->name;
}
