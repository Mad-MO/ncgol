
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



// Function to set the grid size
void set_grid_size(uint16_t width, uint16_t height)
{
    grid_width  = width;
    grid_height = height;
}



// Function to get grid width
uint16_t get_grid_width(void)
{
    return grid_width;
}



// Function to get grid height
uint16_t get_grid_height(void)
{
    return grid_height;
}



// Function to initialize the grid
void init_grid(initpattern_t pattern)
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
        init_pattern_to_grid(PATTERN_CONWAY, grid, 10, 10);
    }
    else if(pattern == INITPATTERN_STILLLIFES)
    {
        // Block
        grid[1][1] = 1;
        grid[1][2] = 1;
        grid[2][1] = 1;
        grid[2][2] = 1;

        // Beehive
        grid[5][2] = 1;
        grid[5][3] = 1;
        grid[6][1] = 1;
        grid[6][4] = 1;
        grid[7][2] = 1;
        grid[7][3] = 1;

        // Loaf
        grid[10][2] = 1;
        grid[11][1] = 1;
        grid[11][3] = 1;
        grid[12][1] = 1;
        grid[12][4] = 1;
        grid[13][2] = 1;
        grid[13][3] = 1;

        // Boat
        grid[16][1] = 1;
        grid[16][2] = 1;
        grid[17][1] = 1;
        grid[17][3] = 1;
        grid[18][2] = 1;

        // Tub
        grid[21][2] = 1;
        grid[22][1] = 1;
        grid[22][3] = 1;
        grid[23][2] = 1;
    }
    else if(pattern == INITPATTERN_OSCILLATORS)
    {
        // Blinker
        grid[2][1] = 1;
        grid[2][2] = 1;
        grid[2][3] = 1;

        // Toad
        grid[9][3]  = 1;
        grid[10][2] = 1;
        grid[10][3] = 1;
        grid[11][2] = 1;
        grid[11][3] = 1;
        grid[12][2] = 1;

        // Beacon
        grid[18][1] = 1;
        grid[18][2] = 1;
        grid[19][1] = 1;
        grid[20][4] = 1;
        grid[21][3] = 1;
        grid[21][4] = 1;

        // Penta-decathlon
        grid[4][11]  = 1;
        grid[5][11]  = 1;
        grid[6][10]  = 1;
        grid[6][12]  = 1;
        grid[7][11]  = 1;
        grid[8][11]  = 1;
        grid[9][11]  = 1;
        grid[10][11] = 1;
        grid[11][10] = 1;
        grid[11][12] = 1;
        grid[12][11] = 1;
        grid[13][11] = 1;
    }
    else if(pattern == INITPATTERN_GLIDER)
    {
        grid[1][3] = 1;
        grid[2][1] = 1;
        grid[2][3] = 1;
        grid[3][2] = 1;
        grid[3][3] = 1;
    }
    else if(pattern == INITPATTERN_SPACESHIPS)
    {
        // Lightweight spaceship (LWSS)
        grid[1][1] = 1;
        grid[1][3] = 1;
        grid[2][4] = 1;
        grid[3][4] = 1;
        grid[4][1] = 1;
        grid[4][4] = 1;
        grid[5][2] = 1;
        grid[5][3] = 1;
        grid[5][4] = 1;

        // Middleweight spaceship (MWSS)
        grid[1][11] = 1;
        grid[1][13] = 1;
        grid[2][14] = 1;
        grid[3][10] = 1;
        grid[3][14] = 1;
        grid[4][14] = 1;
        grid[5][11] = 1;
        grid[5][14] = 1;
        grid[6][12] = 1;
        grid[6][13] = 1;
        grid[6][14] = 1;

        // Heavyweight spaceship (HWSS)
        grid[1][21] = 1;
        grid[1][23] = 1;
        grid[2][24] = 1;
        grid[3][20] = 1;
        grid[3][24] = 1;
        grid[4][20] = 1;
        grid[4][24] = 1;
        grid[5][24] = 1;
        grid[6][21] = 1;
        grid[6][24] = 1;
        grid[7][22] = 1;
        grid[7][23] = 1;
        grid[7][24] = 1;
    }
    else if(pattern == INITPATTERN_GLIDERGUN)
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
    else if(pattern == INITPATTERN_PENTOMINO)
    {
        grid[0+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][0+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[2+(grid_width/2)][0+(grid_height/2)] = 1;
    }
    else if(pattern == INITPATTERN_DIEHARD)
    {
        grid[0+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[7+(grid_width/2)][5+(grid_height/2)] = 1;
    }
    else if(pattern == INITPATTERN_ACORN)
    {
        grid[0+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[4+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][4+(grid_height/2)] = 1;
    }
    else if(pattern == INITPATTERN_BLOCKENGINE1)
    {
        grid[1+(grid_width/2)][6+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][6+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[7+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[7+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[7+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[8+(grid_width/2)][2+(grid_height/2)] = 1;
    }
    else if(pattern == INITPATTERN_BLOCKENGINE2)
    {
        grid[1+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[2+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[2+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[4+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][5+(grid_height/2)] = 1;
    }
    else if(pattern == INITPATTERN_DOUBLEBLOCKENGINE)
    {
        grid[1+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[2+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[4+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[7+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[8+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[10+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[11+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[12+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[13+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[14+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[18+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[19+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[20+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[27+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[28+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[29+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[30+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[31+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[32+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[33+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[35+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[36+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[37+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[38+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[39+(grid_width/2)][1+(grid_height/2)] = 1;
    }
    else            // INITPATTERN_CLEAR
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



// Return text string for pattern
const char * get_pattern_str(initpattern_t pattern)
{
    if     (pattern == INITPATTERN_RANDOM)
        return "Random";
    else if(pattern == INITPATTERN_STILLLIFES)
        return "Still lifes";
    else if(pattern == INITPATTERN_OSCILLATORS)
        return "Oscillators";
    else if(pattern == INITPATTERN_GLIDER)
        return "Glider";
    else if(pattern == INITPATTERN_SPACESHIPS)
        return "Spaceships";
    else if(pattern == INITPATTERN_GLIDERGUN)
        return "Glider gun";
    else if(pattern == INITPATTERN_PENTOMINO)
        return "Pentomino";
    else if(pattern == INITPATTERN_DIEHARD)
        return "Diehard";
    else if(pattern == INITPATTERN_ACORN)
        return "Acorn";
    else if(pattern == INITPATTERN_BLOCKENGINE1)
        return "Block engine 1";
    else if(pattern == INITPATTERN_BLOCKENGINE2)
        return "Block engine 2";
    else if(pattern == INITPATTERN_DOUBLEBLOCKENGINE)
        return "Double block engine";
    else
        return "?";
}



// Function to detect the end of the simulation
uint8_t end_detection(void)
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
