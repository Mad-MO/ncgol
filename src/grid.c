
// File:    grid.c
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the grid functions for the Game of Life
//
// Rules:   https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "grid.h"
#include "patterns.h"



// Create the grid to represent the cells
static uint8_t  grid[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];
static uint8_t  grid_new[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];
static uint32_t cells_alive = 0;
static uint32_t cycle_counter = 0;
#define END_DET_CNT 250
static uint8_t  end_det[END_DET_CNT];
static uint8_t  end_det_pos;
static uint16_t grid_width;
static uint16_t grid_height;
static uint8_t  end_detected = 0;

// Function to detect the end of the simulation
static void handle_end_detection(void);



// Function to set the grid size
void grid_set_size(uint16_t width, uint16_t height)
{
    // Check boundaries
    if(width  > GRID_WIDTH_MAX)  width  = GRID_WIDTH_MAX;
    if(height > GRID_HEIGHT_MAX) height = GRID_HEIGHT_MAX;

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



// Text  strings for the initpattern_t enum
static const char *init_str[][2] =
{
    {"random",    "Random (fire)"},
    {"conway",    "Conway's Game of Life"},
    {"still",     "Still lifes"},
    {"osc",       "Oscillators"},
    {"ships",     "Spaceships"},
    {"gosper",    "Gosper Glider gun"},
    {"simkin",    "Simkin Glider gun"},
    {"pentomino", "Pentomino"},
    {"diehard",   "Die hard"},
    {"acorn",     "Acorn"},
    {"block1",    "Block engine 1"},
    {"block2",    "Block engine 2"},
    {"block3",    "Double block engine"},
    {"8bit",      "I love 8-bit"}
};



// Function to initialize the grid
void grid_init(initpattern_t pattern)
{
    if(pattern >= INITPATTERN_MAX)
        return;

    memset(grid, 0, sizeof(grid));
    memset(grid_new, 0, sizeof(grid_new));
    end_detected = 0;

    if     (pattern == INITPATTERN_RANDOM)
    {
        uint16_t x, y;
        for(x=0; x<grid_width; x++)
            for(y=0; y<grid_height; y++)
                grid[x][y] = (random() & 1);
    }
    else if(pattern == INITPATTERN_CONWAY)
    {
        if((grid_get_width() >= patterns_get_width(PATTERN_CONWAY_FULL)) && (grid_get_height() >= patterns_get_height(PATTERN_CONWAY_FULL)))
        {
            // Conway's Game of Life
            patterns_set_to_center(PATTERN_CONWAY_FULL, grid);
        }
        else
        {
            // Conway
            patterns_set_to_center(PATTERN_CONWAY, grid);
        }
    }
    else if(pattern == INITPATTERN_STILLLIFES)
    {
        // Block
        patterns_set_to_pos(PATTERN_BLOCK, grid, 1, 1);

        // Beehive
        patterns_set_to_pos(PATTERN_BEEHIVE, grid, 6, 1);

        // Loaf
        if(grid_width >= 18)
            patterns_set_to_pos(PATTERN_LOAF, grid, 13, 1);

        // Boat
        if(grid_width >= 24)
            patterns_set_to_pos(PATTERN_BOAT, grid, 20, 1);

        // Tub
        if(grid_width >= 30)
            patterns_set_to_pos(PATTERN_TUB, grid, 26, 1);
    }
    else if(pattern == INITPATTERN_OSCILLATORS)
    {
        // Blinker
        patterns_set_to_pos(PATTERN_BLINKER, grid, 1, 2);

        // Toad
        patterns_set_to_pos(PATTERN_TOAD, grid, 7, 2);

        // Beacon
        if(grid_width >= 18)
            patterns_set_to_pos(PATTERN_BEACON, grid, 14, 2);

        // Pulsar
        if((grid_width >= 36) && (grid_height >= 16))
            patterns_set_to_pos(PATTERN_PULSAR, grid, 22, 1);

        // Penta-decathlon
        if((grid_width >= 17) && (grid_height >= 17))
            patterns_set_to_pos(PATTERN_PENTA_DECATHLON, grid, 4, 10);

        // Octagon
        if(grid_height >= 25)
            patterns_set_to_pos(PATTERN_OCTAGON, grid, 2, 18);

        // Tumbler
        if((grid_width >= 21) && (grid_height >= 25))
            patterns_set_to_pos(PATTERN_TUMBLER, grid, 11, 18);
    }
    else if(pattern == INITPATTERN_SPACESHIPS)
    {
        // Glider
        patterns_set_to_pos(PATTERN_GLIDER, grid, 1, 1);

        // Lightweight spaceship (LWSS)
        patterns_set_to_pos(PATTERN_LWSS, grid, 7, 1);

        // Middleweight spaceship (MWSS)
        if(grid_height >= 14)
            patterns_set_to_pos(PATTERN_MWSS, grid, 7, 7);

        // Heavyweight spaceship (HWSS)
        if(grid_height >= 21)
            patterns_set_to_pos(PATTERN_HWSS, grid, 7, 14);
    }
    else if(pattern == INITPATTERN_GOSPER_GLIDERGUN)
    {
        // Gosper Glider gun
        patterns_set_to_pos(PATTERN_GOSPER_GLIDERGUN, grid, 1, 1);

        // Glider stopper below (move it to the lower right corner)
        if((grid_width >= 38) && (grid_height >= 18))
        {
            #define X_OFFSET 13
            #define Y_OFFSET 0
            uint8_t w=patterns_get_width(PATTERN_GLIDER_STOPPER_BELOW);
            uint8_t h=patterns_get_height(PATTERN_GLIDER_STOPPER_BELOW);
            uint16_t imax=(grid_width>grid_height ? grid_width : grid_height);

            // Search for the best (most distant) position for the stopper
            // -> This is not an efficient solution, but it is the easiest to implement and understand
            for(int i=0; i<imax; i++)
            {
                uint16_t x, y;
                uint8_t w=patterns_get_width(PATTERN_GLIDER_STOPPER_BELOW);
                uint8_t h=patterns_get_height(PATTERN_GLIDER_STOPPER_BELOW);
                uint16_t imax=(grid_width>grid_height ? grid_width : grid_height);
                if((X_OFFSET+w+i <= grid_width) && (Y_OFFSET+h+i <= grid_height))
                {
                    x=X_OFFSET+i;
                    y=Y_OFFSET+i;
                }
                else
                {
                    patterns_set_to_pos(PATTERN_GLIDER_STOPPER_BELOW, grid, x, y);
                    break;
                }
            }
            #undef X_OFFSET
            #undef Y_OFFSET
        }
    }
    else if(pattern == INITPATTERN_SIMKIN_GLIDERGUN)
    {
        // Simkin Glider gun
        patterns_set_to_center(PATTERN_SIMKIN_GLIDERGUN, grid);

        // Glider stopper below (move it to the lower right corner)
        if((grid_width >= 33) && (grid_height >= 27))
        {
            #define X_OFFSET 3
            #define Y_OFFSET 0
            uint8_t ws=patterns_get_width(PATTERN_GLIDER_STOPPER_BELOW);
            uint8_t hs=patterns_get_height(PATTERN_GLIDER_STOPPER_BELOW);
            uint8_t wp=patterns_get_width(PATTERN_SIMKIN_GLIDERGUN);
            uint8_t hp=patterns_get_height(PATTERN_SIMKIN_GLIDERGUN);
            uint16_t imax=(grid_width>grid_height ? grid_width : grid_height);

            // Search for the best (most distant) position for the stopper
            // -> This is not an efficient solution, but it is the easiest to implement and understand
            for(uint16_t i=0; i<imax; i++)
            {
                uint16_t x, y;
                if((((grid_width-wp)/2)+X_OFFSET+ws+i <= grid_width) && (((grid_height-hp)/2)+Y_OFFSET+hs+i <= grid_height))
                {
                    x=((grid_width-wp)/2)+X_OFFSET+i;
                    y=((grid_height-hp)/2)+Y_OFFSET+i;
                }
                else
                {
                    patterns_set_to_pos(PATTERN_GLIDER_STOPPER_BELOW, grid, x, y);
                    break;
                }
            }
            #undef X_OFFSET
            #undef Y_OFFSET
        }

        // Glider stopper above
        if((grid_width >= 33) && (grid_height >= 30))
        {
            #define X_OFFSET 17
            #define Y_OFFSET 0
            uint8_t ws=patterns_get_width(PATTERN_GLIDER_STOPPER_ABOVE);
            uint8_t hs=patterns_get_height(PATTERN_GLIDER_STOPPER_ABOVE);
            uint8_t wp=patterns_get_width(PATTERN_SIMKIN_GLIDERGUN);
            uint8_t hp=patterns_get_height(PATTERN_SIMKIN_GLIDERGUN);
            uint16_t imax=(grid_width>grid_height ? grid_width : grid_height);

            // Search for the best (most distant) position for the stopper
            // -> This is not an efficient solution, but it is the easiest to implement and understand
            for(uint16_t i=0; i<imax; i++)
            {
                uint16_t x, y;
                if((((grid_width-wp)/2)+X_OFFSET-i >= 0) && (((grid_height-hp)/2)+Y_OFFSET-i >= 0))
                {
                    x=((grid_width-wp)/2)+X_OFFSET-i;
                    y=((grid_height-hp)/2)+Y_OFFSET-i;
                }
                else
                {
                    patterns_set_to_pos(PATTERN_GLIDER_STOPPER_ABOVE, grid, x, y);
                    break;
                }
            }
            #undef X_OFFSET
            #undef Y_OFFSET
        }
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
    else if(pattern == INITPATTERN_ILOVE8BIT)
    {
        // I love 8 bit
        patterns_set_to_center(PATTERN_ILOVE8BIT, grid);
    }
    else            // INITPATTERN_CLEAR
    {
        // Do nothing
    }

    cycle_counter = 0;
}



// Function to update the grid based on the game of life rules (multi-threaded with a subset of given columns for each thread)
typedef struct
{
    uint16_t x_beg;
    uint16_t x_cnt;
} calc_thread_arg_t;

void * grid_calc(void * args)
{
    uint16_t x, y;
    uint16_t x_beg = ((calc_thread_arg_t*)args)->x_beg;
    uint16_t x_cnt = ((calc_thread_arg_t*)args)->x_cnt;

    for(x=x_beg; x<(x_beg+x_cnt); x++)
    {
        for(y=0; y<grid_height; y++)
        {
            // Count neighbors
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
            // Calculate new state
            // -> This way seems redundant (2 times the same else case), but it is more cpu efficient!
            if(grid[x][y] == 0)
            {
                if(neighbors == 3)      // Reproduction
                    grid_new[x][y] = 1;
                else                    // Stasis
                    grid_new[x][y] = grid[x][y];
            }
            else
            {
                if     (neighbors  < 2) // Underpopulation
                    grid_new[x][y] = 0;
                else if(neighbors  > 3) // Overpopulation
                    grid_new[x][y] = 0;
                else                    // Stasis
                    grid_new[x][y] = grid[x][y];
            }
        }
    }
    pthread_exit(NULL);
}



// Function to update the grid based on the game of life rules (start multi-threaded calculation)
void grid_update(void)
{
    uint16_t thread_cnt = sysconf(_SC_NPROCESSORS_ONLN); // Number of active Cores
    if(thread_cnt > grid_width) thread_cnt = grid_width;
    pthread_t threads[thread_cnt];
    calc_thread_arg_t args[thread_cnt];

    if(!end_detected)
    {
        cycle_counter++;
    }

    for(int i=0; i<thread_cnt; i++)
    {
        uint16_t x_beg = ((int)grid_width * i) / thread_cnt;
        uint16_t x_end = ((int)grid_width * (i+1)) / thread_cnt;
        uint16_t x_cnt = x_end - x_beg;
        args[i].x_beg = x_beg;
        args[i].x_cnt = x_cnt;
        if(pthread_create(&threads[i], NULL, grid_calc, (void *)&args[i]))
        {
            exit(1);
        }
    }
    for(int i=0; i<thread_cnt; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Count living cells
    uint32_t l_cells_alive = 0;
    for(uint16_t x=0; x<grid_width; x++)
    {
        for(uint16_t y=0; y<grid_height; y++)
        {
            if(grid_new[x][y])
                l_cells_alive++;
        }
    }
    cells_alive = l_cells_alive;

    memcpy(grid, grid_new, sizeof(grid));
    handle_end_detection();
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



// Return short text string for pattern
const char * grid_get_initpattern_short_str(initpattern_t initpattern)
{
    if(initpattern < INITPATTERN_MAX)
    {
        return init_str[initpattern][0];
    }
    else
    {
        return "?";
    }
}



// Return long text string for pattern
const char * grid_get_initpattern_long_str(initpattern_t initpattern)
{
    if(initpattern < INITPATTERN_MAX)
    {
        return init_str[initpattern][1];
    }
    else
    {
        return "?";
    }
}



// Function to detect the end of the simulation
static void handle_end_detection(void)
{
    end_det_pos++;
    end_det_pos %= END_DET_CNT;
    end_det[end_det_pos] = cells_alive;
    if(cells_alive == 0)
    {
        end_detected = 1;
    }
    else if(cycle_counter > END_DET_CNT)                               // At least END_DET_CNT cycles needed for detection
    {
        for(uint8_t sequence=1; sequence<=(END_DET_CNT/2); sequence++) // Test sequence in the length of 1 to half of the buffer
        {
            for(uint8_t testpos=sequence; testpos<END_DET_CNT; testpos++)
            {
                if(end_det[testpos] != end_det[testpos % sequence])    // Pattern not found? -> End loop and test next sequence
                    break;
                if(testpos == END_DET_CNT - 1)                         // End of loop reached? -> Pattern found!
                {
                    if(!end_detected)
                    {
                        cycle_counter -= END_DET_CNT;                  // Reset cycle counter to the point where the pattern was detected
                    }
                    end_detected = 1;
                }
            }
        }
    }
}



// Return if end of simulation has been detected
uint8_t grid_end_detected(void)
{
    return end_detected;
}
