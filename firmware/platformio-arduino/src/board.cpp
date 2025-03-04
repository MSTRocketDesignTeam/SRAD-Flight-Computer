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

        // Initialize the Pyro Channels 
        gpioInit(PIN::CH1_FIRE, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S);
        gpioInit(PIN::CH2_FIRE, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S);
        gpioInit(PIN::CH1_DETECT, PIN_MODE::INPUT_M, PIN_STATE::LOW_S); 
        gpioInit(PIN::CH2_DETECT, PIN_MODE::INPUT_M, PIN_STATE::LOW_S); 

        // The buzzer has some issues currently, Tristate the pin to disable it
        gpioInit(PIN::BUZZER, PIN_MODE::INPUT_M, PIN_STATE::INPUT_HI_Z_S); 

        // Initialize CDC Serial for debugging throughout the program
        Serial.begin(9600);

        // Initialize all of the chip selects 
        gpioInit(PIN::HG_ACCEL_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::IMU_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::BARO_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::TEL_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::FRAM_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 

        // Disable the LEDs 
        gpioInit(PIN::LED_R, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);
        gpioInit(PIN::LED_G, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);
        gpioInit(PIN::LED_B, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);

        // Initialize the SPI interface that is used by most sensors 
        gpioInit(PIN::CH1_FIRE, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // spi.begin checks this
        gpioInit(PIN::CH2_FIRE, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); 

        SPI.begin(); 
        return; 
}
/* -------------------------------------------------------------------------- */