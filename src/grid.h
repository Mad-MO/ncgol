
// File:    grid.h
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the grid functions for the Game of Life

#ifndef __GRID_H
#define __GRID_H

#include <stdint.h>



// Define the size of the grid
// Example: Fullscreen Terminal on Ultrawidescreen Monitor
//          -> ~569x110 characters
//          -> 1134x420 cells (when using braille style)
#define GRID_WIDTH_MAX  2500 // 2500*1000*2 = ~5 MB
#define GRID_HEIGHT_MAX 1000

// Pointertye to: uint8_t grid[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];
typedef uint8_t grid_t[GRID_HEIGHT_MAX];



typedef enum
{
    INITPATTERN_RANDOM,
    INITPATTERN_CONWAY,
    INITPATTERN_STILLLIFES,
    INITPATTERN_OSCILLATORS,
    INITPATTERN_GLIDER,
    INITPATTERN_SPACESHIPS,
    INITPATTERN_GLIDERGUN,
    INITPATTERN_PENTOMINO,
    INITPATTERN_DIEHARD,
    INITPATTERN_ACORN,
    INITPATTERN_BLOCKENGINE1,
    INITPATTERN_BLOCKENGINE2,
    INITPATTERN_DOUBLEBLOCKENGINE,
    // ----------------
    INITPATTERN_CYCLEMAX, // Boundary for cycling through patterns
    INITPATTERN_CLEAR,    // Special pattern to clear the grid
    INITPATTERN_MAX
} initpattern_t;



// Function to set the grid size
void grid_set_size(uint16_t width, uint16_t height);

// Function to get grid width
uint16_t grid_get_width(void);

// Function to get grid height
uint16_t grid_get_height(void);

// Function to initialize the grid
void grid_init(initpattern_t pattern);

// Function to update the grid based on the game of life rules
void grid_update(void);

// Get pointer to grid
grid_t * grid_get(void);

// Get count of cells which are alive
uint32_t grid_get_cells_alive(void);

// Get cycle counter
uint32_t grid_get_cycle_counter(void);

// Return text string for initpattern
const char* grid_get_initpattern_str(initpattern_t initpattern);

// Return if end of simulation has been detected
uint8_t grit_end_detected(void);

#endif // __GRID_H
