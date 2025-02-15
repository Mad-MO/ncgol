
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

static PatternType pattern;



// Function to set the grid size
void set_grid_size(uint16_t width, uint16_t height)
{
    grid_width  = width;
    grid_height = height;
}



// Function to initialize the grid
void init_grid(PatternType pattern)
{
    if(pattern < PatternTypeMax)
        pattern = pattern;
    else
        return;

    memset(grid, 0, sizeof(grid));

    if     (pattern == PatternTypeRandom)
    {
        uint16_t x, y;
        for(x=0; x<grid_width; x++)
            for(y=0; y<grid_height; y++)
                grid[x][y] = (random() & 1);
    }
    else if(pattern == PatternTypeOscillators)
    {
        // Blinker
        grid[2][1] = 1;
        grid[2][2] = 1;
        grid[2][3] = 1;

        // Toad
        grid[9][2] = 1;
        grid[10][1] = 1;
        grid[10][2] = 1;
        grid[11][1] = 1;
        grid[11][2] = 1;
        grid[12][1] = 1;

        // Beacon
        grid[18][1] = 1;
        grid[18][2] = 1;
        grid[19][1] = 1;
        grid[20][4] = 1;
        grid[21][3] = 1;
        grid[21][4] = 1;

        // Penta-decathlon
        grid[4][10]  = 1;
        grid[5][10]  = 1;
        grid[6][9]   = 1;
        grid[6][11]  = 1;
        grid[7][10]  = 1;
        grid[8][10]  = 1;
        grid[9][10]  = 1;
        grid[10][10] = 1;
        grid[11][9]  = 1;
        grid[11][11] = 1;
        grid[12][10] = 1;
        grid[13][10] = 1;
    }
    else if(pattern == PatternTypeGlider)
    {
        grid[1][3] = 1;
        grid[2][1] = 1;
        grid[2][3] = 1;
        grid[3][2] = 1;
        grid[3][3] = 1;
    }
    else if(pattern == PatternTypeGliderGun)
    {
        grid[1][5] = 1;
        grid[1][6] = 1;
        grid[2][5] = 1;
        grid[2][6] = 1;
        grid[11][5] = 1;
        grid[11][6] = 1;
        grid[11][7] = 1;
        grid[12][4] = 1;
        grid[12][8] = 1;
        grid[13][3] = 1;
        grid[13][9] = 1;
        grid[14][3] = 1;
        grid[14][9] = 1;
        grid[15][6] = 1;
        grid[16][4] = 1;
        grid[16][8] = 1;
        grid[17][5] = 1;
        grid[17][6] = 1;
        grid[17][7] = 1;
        grid[18][6] = 1;
        grid[21][3] = 1;
        grid[21][4] = 1;
        grid[21][5] = 1;
        grid[22][3] = 1;
        grid[22][4] = 1;
        grid[22][5] = 1;
        grid[23][2] = 1;
        grid[23][6] = 1;
        grid[25][1] = 1;
        grid[25][2] = 1;
        grid[25][6] = 1;
        grid[25][7] = 1;
        grid[35][3] = 1;
        grid[35][4] = 1;
        grid[36][3] = 1;
        grid[36][4] = 1;
    }
    else if(pattern == PatternTypePentomino)
    {
        grid[0+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][0+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[2+(grid_width/2)][0+(grid_height/2)] = 1;
    }
    else if(pattern == PatternTypeDiehard)
    {
        grid[0+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[7+(grid_width/2)][5+(grid_height/2)] = 1;
    }
    else if(pattern == PatternTypeAcorn)
    {
        grid[0+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[4+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][4+(grid_height/2)] = 1;
    }
    else            // PatternTypeClear
    {
        // Do nothing
    }

    cycle_counter = 0;
}



// Function to update the grid based on the game of life rules
void update_grid(void)
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
}



// Get pointer to grid
grid_t * get_grid(void)
{
    return grid;
}



// Get count of cells which are alive
uint32_t get_cells_alive(void)
{
    return cells_alive;
}



// Get cycle counter
uint32_t get_cycle_counter(void)
{
    return cycle_counter;
}



// Function to detect the end of the simulation
uint8_t end_detection(void)
{
    end_det_pos++;
    end_det_pos %= END_DET_CNT;
    end_det[end_det_pos] = cells_alive;
    if(cells_alive == 0)
        return 1;
    if(cycle_counter > END_DET_CNT)                                 // At least END_DET_CNT cycles needed for detection
    {
        for(uint8_t pattern=1; pattern<=(END_DET_CNT/2); pattern++) // Test pattern in the length of 1 to half of the buffer
        {
            for(uint8_t testpos=pattern; testpos<END_DET_CNT; testpos++)
            {
                if(end_det[testpos] != end_det[testpos % pattern])  // Pattern not found? -> End loop and test next pattern
                    break;
                if(testpos == END_DET_CNT - 1)                      // End of loop reached? -> Pattern found!
                    return 1;
            }
        }
    }
    return 0;
}
