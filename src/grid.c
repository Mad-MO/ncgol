
// File:    grid.c
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the grid functions for the Game of Life
//
// Rules:   https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "grid.h"
#include "patterns.h"



// Create the grid to represent the cells
static uint8_t  grid[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];
static uint8_t  new_grid[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];
static uint32_t cells_alive = 0;
static uint32_t cycle_counter = 0;
#define END_DET_CNT 60
static uint8_t  end_det[END_DET_CNT];
static uint8_t  end_det_pos;
static uint16_t grid_width;
static uint16_t grid_height;
static uint8_t  end_detected = 0;

// Function to detect the end of the simulation
static uint8_t handle_end_detection(void);



// Function to set the grid size
void grid_set_size(uint16_t width, uint16_t height)
{
    grid_width  = width;
    grid_height = height;
}



// Function to get grid width
uint16_t grid_get_width(void)
{
    return grid_width;
}



// Function to get grid height
uint16_t grid_get_height(void)
{
    return grid_height;
}



// Function to initialize the grid
void grid_init(initpattern_t pattern)
{
    if(pattern >= INITPATTERN_MAX)
        return;

    memset(grid, 0, sizeof(grid));

    if     (pattern == INITPATTERN_RANDOM)
    {
        uint16_t x, y;
        for(x=0; x<grid_width; x++)
            for(y=0; y<grid_height; y++)
                grid[x][y] = (random() & 1);
    }
    else if(pattern == INITPATTERN_CONWAY)
    {
        // Conway
        patterns_set_to_center(PATTERN_CONWAY, grid);
    }
    else if(pattern == INITPATTERN_STILLLIFES)
    {
        // Block
        patterns_set_to_pos(PATTERN_BLOCK, grid, 1, 1);

        // Beehive
        patterns_set_to_pos(PATTERN_BEEHIVE, grid, 5, 1);

        // Loaf
        patterns_set_to_pos(PATTERN_LOAF, grid, 11, 1);

        // Boat
        patterns_set_to_pos(PATTERN_BOAT, grid, 17, 1);

        // Tub
        patterns_set_to_pos(PATTERN_TUB, grid, 22, 1);
    }
    else if(pattern == INITPATTERN_OSCILLATORS)
    {
        // Blinker
        patterns_set_to_pos(PATTERN_BLINKER, grid, 1, 2);

        // Toad
        patterns_set_to_pos(PATTERN_TOAD, grid, 6, 2);

        // Beacon
        patterns_set_to_pos(PATTERN_BEACON, grid, 12, 2);

        // Penta-decathlon
        patterns_set_to_pos(PATTERN_PENTA_DECATHLON, grid, 4, 10);
    }
    else if(pattern == INITPATTERN_GLIDER)
    {
        // Glider
        patterns_set_to_pos(PATTERN_GLIDER, grid, 1, 1);
    }
    else if(pattern == INITPATTERN_SPACESHIPS)
    {
        // Lightweight spaceship (LWSS)
        patterns_set_to_pos(PATTERN_LWSS, grid, 1, 1);

        // Middleweight spaceship (MWSS)
        patterns_set_to_pos(PATTERN_MWSS, grid, 1, 7);

        // Heavyweight spaceship (HWSS)
        patterns_set_to_pos(PATTERN_HWSS, grid, 1, 14);
    }
    else if(pattern == INITPATTERN_GLIDERGUN)
    {
        // Glider gun
        patterns_set_to_pos(PATTERN_GLIDERGUN, grid, 1, 1);
    }
    else if(pattern == INITPATTERN_PENTOMINO)
    {
        // Pentomino
        patterns_set_to_center(PATTERN_PENTOMINO, grid);
    }
    else if(pattern == INITPATTERN_DIEHARD)
    {
        // Diehard
        patterns_set_to_center(PATTERN_DIEHARD, grid);
    }
    else if(pattern == INITPATTERN_ACORN)
    {
        // Acorn
        patterns_set_to_center(PATTERN_ACORN, grid);
    }
    else if(pattern == INITPATTERN_BLOCKENGINE1)
    {
        // Block engine 1
        patterns_set_to_center(PATTERN_BLOCKENGINE1, grid);
    }
    else if(pattern == INITPATTERN_BLOCKENGINE2)
    {
        // Block engine 2
        patterns_set_to_center(PATTERN_BLOCKENGINE2, grid);
    }
    else if(pattern == INITPATTERN_DOUBLEBLOCKENGINE)
    {
        // Double block engine
        patterns_set_to_center(PATTERN_DOUBLEBLOCKENGINE, grid);
    }
    else            // INITPATTERN_CLEAR
    {
        // Do nothing
    }

    cycle_counter = 0;
}



// Function to update the grid based on the game of life rules
void grid_update(void)
{
    uint16_t x, y;

    memset(new_grid, 0, sizeof(new_grid));
    cells_alive = 0;
    cycle_counter++;

    for(x=0; x<grid_width; x++)
    {
        for(y=0; y<grid_height; y++)
        {
            uint8_t neighbors = 0;
            for(int8_t dx=-1; dx<=1; dx++)
            {
                for(int8_t dy=-1; dy<=1; dy++)
                {
                    if(dx == 0 && dy == 0)
                    {
                        continue;
                    }
                    uint16_t nx = (grid_width + x + dx) % grid_width;
                    uint16_t ny = (grid_height + y + dy) % grid_height;
                    neighbors += grid[nx][ny];
                }
            }

            if     ((grid[x][y] == 1) && (neighbors  < 2)) // Underpopulation
                new_grid[x][y] = 0;
            else if((grid[x][y] == 1) && (neighbors  > 3)) // Overpopulation
                new_grid[x][y] = 0;
            else if((grid[x][y] == 0) && (neighbors == 3)) // Reproduction
                new_grid[x][y] = 1;
            else                                           // Stasis
                new_grid[x][y] = grid[x][y];

            if(new_grid[x][y])
                cells_alive++;
        }
    }

    memcpy(grid, new_grid, sizeof(grid));

    end_detected = handle_end_detection();
}



// Get pointer to grid
grid_t * grid_get(void)
{
    return grid;
}



// Get count of cells which are alive
uint32_t grid_get_cells_alive(void)
{
    return cells_alive;
}



// Get cycle counter
uint32_t grid_get_cycle_counter(void)
{
    return cycle_counter;
}



// Return text string for pattern
const char* grid_get_initpattern_str(initpattern_t initpattern)
{
    if     (initpattern == INITPATTERN_RANDOM)
        return "Random";
    else if(initpattern == INITPATTERN_CONWAY)
        return patterns_get_str(PATTERN_CONWAY);
    else if(initpattern == INITPATTERN_STILLLIFES)
        return "Still lifes";
    else if(initpattern == INITPATTERN_OSCILLATORS)
        return "Oscillators";
    else if(initpattern == INITPATTERN_GLIDER)
        return patterns_get_str(PATTERN_GLIDER);
    else if(initpattern == INITPATTERN_SPACESHIPS)
        return "Spaceships";
    else if(initpattern == INITPATTERN_GLIDERGUN)
        return patterns_get_str(PATTERN_GLIDERGUN);
    else if(initpattern == INITPATTERN_PENTOMINO)
        return patterns_get_str(PATTERN_PENTOMINO);
    else if(initpattern == INITPATTERN_DIEHARD)
        return patterns_get_str(PATTERN_DIEHARD);
    else if(initpattern == INITPATTERN_ACORN)
        return patterns_get_str(PATTERN_ACORN);
    else if(initpattern == INITPATTERN_BLOCKENGINE1)
        return patterns_get_str(PATTERN_BLOCKENGINE1);
    else if(initpattern == INITPATTERN_BLOCKENGINE2)
        return patterns_get_str(PATTERN_BLOCKENGINE2);
    else if(initpattern == INITPATTERN_DOUBLEBLOCKENGINE)
        return patterns_get_str(PATTERN_DOUBLEBLOCKENGINE);
    else
        return "?";
}



// Function to detect the end of the simulation
static uint8_t handle_end_detection(void)
{
    end_det_pos++;
    end_det_pos %= END_DET_CNT;
    end_det[end_det_pos] = cells_alive;
    if(cells_alive == 0)
        return 1;
    if(cycle_counter > END_DET_CNT)                                    // At least END_DET_CNT cycles needed for detection
    {
        for(uint8_t sequence=1; sequence<=(END_DET_CNT/2); sequence++) // Test sequence in the length of 1 to half of the buffer
        {
            for(uint8_t testpos=sequence; testpos<END_DET_CNT; testpos++)
            {
                if(end_det[testpos] != end_det[testpos % sequence])    // Pattern not found? -> End loop and test next sequence
                    break;
                if(testpos == END_DET_CNT - 1)                         // End of loop reached? -> Pattern found!
                    return 1;
            }
        }
    }
    return 0;
}



// Return if end of simulation has been detected
uint8_t grit_end_detected(void)
{
    return end_detected;
}
