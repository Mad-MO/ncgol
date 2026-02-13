
// File:    end_det.h
// Author:  Martin Ochs
// License: MIT
// Brief:   Implementation of the end detection functions for the Game of Life

#ifndef __END_DET_H
#define __END_DET_H

#include <stdint.h>



// Function to detect the end of the simulation
void end_det_handle(uint32_t alive);

// Function to reset the end detection
void end_det_reset(void);

// Return "1" if end of simulation has been detected
uint8_t end_det_detected(void);

// Return number of cycles since beginning of the end detection
uint32_t end_det_get_detection_cycles(void);



#endif // __END_DET_H
