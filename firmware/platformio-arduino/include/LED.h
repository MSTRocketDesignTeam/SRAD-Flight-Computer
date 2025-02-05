/*
Some basic wrappers for controlling the LEDs
*/

#ifndef LED_H
#define LED_H 
#include "board.h"
#include <stdint.h>

/* ----------------------------------- LED ---------------------------------- */
void SET_R(const uint8_t state);
void SET_G(const uint8_t state);
void SET_B(const uint8_t state);
/* -------------------------------------------------------------------------- */

#endif 