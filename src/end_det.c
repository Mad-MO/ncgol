
// File:    patterns.c
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the patterns functions for the Game of Life

#include <stdint.h>
#include <string.h>
#include "end_det.h"

#define END_DET_CNT_MAX 500
#define END_DET_CNT_MIN  20
static uint16_t end_det[END_DET_CNT_MAX];
static uint16_t end_det_pos  = 0;
static uint8_t  end_detected = 0;
static uint32_t end_det_cycles = 0;



// Function to reset the end detection
void end_det_reset(void)
{
    end_det_pos    = 0;
    end_det_cycles = 0;
    end_detected   = 0;

    memset(end_det, 0, sizeof(end_det));
}



// Function to detect the end of the simulation
void end_det_handle(uint32_t alive)
{
    uint16_t testlength = 0;

    end_det_cycles++;
    end_det[end_det_pos] = alive;
    end_det_pos++;
    end_det_pos %= END_DET_CNT_MAX;

    if(alive == 0)
    {
        end_detected = 1;
    }
    else if(end_det_cycles > END_DET_CNT_MIN)                               // At least END_DET_CNT_MIN cycles needed for detection
    {
        for(uint16_t sequence=1; sequence<=(END_DET_CNT_MAX/2); sequence++) // Test sequence in the length of 1 to half of the buffer
        {
            for(uint16_t testpos=sequence; testpos<END_DET_CNT_MAX; testpos++)
            {
                if(end_det[testpos] != end_det[testpos % sequence])         // Pattern not found? -> End loop and test next sequence
                    break;
                if(testpos == END_DET_CNT_MAX - 1)                          // End of loop reached? -> Pattern found!
                {
                    end_detected = 1;
                }
            }
        }
    }
}



// Return if end of simulation has been detected
uint8_t end_det_detected(void)
{
    return end_detected;
}
