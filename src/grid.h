
// File:    grid.h
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the grid functions for the Game of Life

#include <stdint.h>



// Define the size of the grid
// Example: Fullscreen Terminal on Ultrawidescreen Monitor
//          -> ~569x110 characters
//          -> 1134x420 cells (when using braille style)
#define GRID_WIDTH_MAX  2500 // 2500*1000*2 = ~5 MB
#define GRID_HEIGHT_MAX 1000

// Pointertye to: uint8_t grid[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];
typedef const uint8_t grid_t[GRID_HEIGHT_MAX];



typedef enum
{
    PatternTypeRandom,
    PatternTypeStillLifes,
    PatternTypeOscillators,
    PatternTypeGlider,
    PatternTypeGliderGun,
    PatternTypePentomino,
    PatternTypeDiehard,
    PatternTypeAcorn,
    // ----------------
    PatternTypeCycleMax, // Boundary for cycling through patterns
    PatternTypeClear,    // Special pattern to clear the grid
    PatternTypeMax
} PatternType;



// Function to set the grid size
void set_grid_size(uint16_t width, uint16_t height);

// Function to initialize the grid
void init_grid(PatternType pattern);

// Function to update the grid based on the game of life rules
void update_grid(void);

// Get pointer to grid
grid_t * get_grid(void);

// Get count of cells which are alive
uint32_t get_cells_alive(void);

// Get cycle counter
uint32_t get_cycle_counter(void);

// Return text string for pattern
const char * get_pattern_str(PatternType pattern);

// Function to detect the end of the simulation
uint8_t end_detection(void);
