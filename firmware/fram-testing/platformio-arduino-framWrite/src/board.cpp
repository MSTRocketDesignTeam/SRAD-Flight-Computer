/*
        This is the implementation file for any functions from the board.h file
*/

#include "board.h"
#include <avr/io.h>
#include "gpio.h"
#include <Arduino.h>
#include <SPI.h>

/* --------------------------- FUNCTION_DEFINTIONS -------------------------- */
void boardInit()
{
        // because of how the SRAD-FC has its USB port connected, we must disable the
        // internal USB voltage regulator
        UHWCON &= ~(1 << UVREGE); // SRAD Board (disable regulator)

        // The buzzer has some issues currently, Tristate the pin to disable it
        gpioInit(PIN::BUZZER, PIN_MODE::INPUT_M, PIN_STATE::INPUT_HI_Z_S); 

        // Initialize CDC Serial for debugging throughout the program
        Serial.begin(9600);

        // Initialize the SPI interface that is used by most sensors 
        gpioInit(PIN::CH1_FIRE, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // spi.begin checks this
        SPI.begin(); 
}
/* -------------------------------------------------------------------------- */