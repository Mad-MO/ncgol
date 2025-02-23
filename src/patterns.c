
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

    // Check for null-pointer
    if(grid == 0)
    {
        return;
    }

    // Check boundaries
    if(pattern >= PATTERN_MAX)
    {
        return;
    }

    // Copy pattern to grid
    for(x=0; x<pattern_desc->width; x++)
    {
        for(y=0; y<pattern_desc->height; y++)
        {
            if((x+x_pos < grid_get_width()) && (y+y_pos < grid_get_height())) // Check boundaries
            {
                grid[x+x_pos][y+y_pos] = pattern_desc->pattern[x + y*pattern_desc->width];
            }
        }
    }
}



void patterns_set_to_center(pattern_t pattern, grid_t *grid)
{
    // Check for null-pointer
    if(grid == 0)
    {
        return;
    }

    // Check boundaries
    if(pattern >= PATTERN_MAX)
    {
        return;
    }

    // Calculate center position
    uint16_t x_pos;
    uint16_t y_pos;
    if(pattern_list[pattern]->width >= grid_get_width())
    {
        x_pos = 0;
    }
    else
    {
        x_pos = (grid_get_width()  - pattern_list[pattern]->width)  / 2;
    }
    if(pattern_list[pattern]->height >= grid_get_height())
    {
        y_pos = 0;
    }
    else
    {
        y_pos = (grid_get_height() - pattern_list[pattern]->height) / 2;
    }

    // Copy pattern to grid
    patterns_set_to_pos(pattern, grid, x_pos, y_pos);
}

// Return text string for initpattern
const char * patterns_get_str(pattern_t pattern)
{
    if(pattern < PATTERN_MAX) // Check boundaries
    {
        return pattern_list[pattern]->name;
    }
    else
    {
        return "?";
    }
}



// Get pattern width
uint16_t patterns_get_width(pattern_t pattern)
{
    if(pattern < PATTERN_MAX) // Check boundaries
    {
        return pattern_list[pattern]->width;
    }
    else
    {
        return 0;
    }
}



// Get pattern height
uint16_t patterns_get_height(pattern_t pattern)
{
    if(pattern < PATTERN_MAX) // Check boundaries
    {
        return pattern_list[pattern]->height;
    }
    else
    {
        return 0;
    }
}
