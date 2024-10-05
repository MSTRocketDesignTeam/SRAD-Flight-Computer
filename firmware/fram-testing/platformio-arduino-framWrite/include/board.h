/*
        This file is a PCB and Microcontroller specific file that specifies how 
the different components on the altimeter are connected together. The hope is that 
in the future if a new altimeter is created, simply changing this file will restore
previous functionality. 

Author: Seth Sievers
Date: 9/28/24
*/

#include <stdint.h>

#ifndef BOARD_H
#define BOARD_H

/* ------------------------------ PIN_CONSTANTS ----------------------------- */
// enum defining which pin number is connected to a function
// these are using Arduino compatible pin numbers 
enum PIN : uint8_t
{
        // RGB LED Pins
        LED_R = 5, 
        LED_G = 9,
        LED_B = 12, 

        // Pyro Pins
        CH1_FIRE = 17, // (RXLED = IO17)
        CH1_DETECT = 8,
        CH2_FIRE = 30, // (TXLED = IO30)
        CH2_DETECT = 10,

        // SPI Pins & CS
        SPI_SCK= 15, 
        SPI_MOSI = 16,
        SPI_MISO = 14, 
        BARO_CS = 20,
        TEL_CS = 21,
        HG_ACCEL_CS = 19,
        FRAM_CS = 23,
        FRAM_HOLD = 22,
        BUZZER = 13
};
/* -------------------------------------------------------------------------- */

/* -------------------------- FUNCTION_DECLARATION -------------------------- */
// Desc: Intiializes some specific register settings for the atmega32u4
// Args: None 
// Returns: Nothing 
void boardInit(); 
/* -------------------------------------------------------------------------- */

#endif