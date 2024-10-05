/*
        Source file for the microcontroller specific GPIO implementation.
*/

#include "gpio.h"
#include "board.h"
#include <stdint.h>
#include <Arduino.h>

/* --------------------------- FUNCTION_DEFINTIONS -------------------------- */
void gpioInit(const PIN pin, const PIN_MODE mode, const PIN_STATE state)
{
        // since this set of files is arduino based, can just call arduino land
        pinMode(pin, mode);
        digitalWrite(pin, state);
        return;  
}

void gpioSet(const PIN pin, const PIN_STATE state)
{
        // arduino-land, use digitalWrite
        digitalWrite(pin, state); 
        return; 
}
/* -------------------------------------------------------------------------- */