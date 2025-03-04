/*
        This file is a microcontroller specific file (like board.h). The GPIO header
and source file will define functions to initialize, set the state of, and read
GPIO pins. These functions must take in the enum defining the pin in board.h. This
Will enable cross MCU compatibility by changing the gpio.h and board.h files on a
per MCU basis
*/

#ifndef GPIO_H
#define GPIO_H

#include "board.h"
#include <stdint.h>

/* ----------------------------- ENUM_CONSTANTS ----------------------------- */
// define whether a pin is an input or output
enum PIN_MODE : uint8_t
{
        INPUT_M = 0, // same as in Arduino.h
        OUTPUT_M // same as in Arduino.h
};

// define whether a pin is high or low 
enum PIN_STATE : uint8_t 
{
        HIGH_S = 1, 
        LOW_S = 0,
        INPUT_PULLUP_S = 1, // input with pullup resistors 
        INPUT_HI_Z_S = 0 // high impedance state
};
/* -------------------------------------------------------------------------- */

/* -------------------------- FUCNTION_DECLARATION -------------------------- */
// Desc: Initialize the pin as I/O and set the initial state
// Args: Enums defining the pin, mode, and state
// Returns: Nothing 
void gpioInit(const PIN pin, const PIN_MODE mode, const PIN_STATE state);

// Desc: Change the state of the pin
// Args: Enums defining the pin and state
// Returns: Nothing 
void gpioSet(const PIN pin, const PIN_STATE state); 

uint8_t gpioRead(const PIN pin);
/* -------------------------------------------------------------------------- */

#endif