#include <Arduino.h>
#include "gpio.h"
#include "board.h"
#include <Adafruit_FRAM_SPI.h>

void setup()
{
        // init various components on the board
        boardInit(); 

        // Set set all (CS) to be HIGH state (disabled)
        gpioInit(PIN::HG_ACCEL_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::IMU_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::BARO_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::TEL_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::FRAM_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 

        // The FRAM !(HOLD) is not used by the current library, set HIGH to disable
        gpioInit(PIN::FRAM_HOLD, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);

        // led pins for debugging 
        gpioInit(PIN::LED_R, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);
        gpioInit(PIN::LED_G, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);
        gpioInit(PIN::LED_B, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);

        // send init message
        gpioSet(PIN::LED_R, PIN_STATE::LOW_S); 
        delay(5000); 
        Serial.println(F("board init complete")); 
        gpioSet(PIN::LED_R, PIN_STATE::HIGH_S);

        // initalize the FRAM 
        Adafruit_FRAM_SPI fram; 
        fram.begin(); 


        while (true) { ; } // never end 
        return; 
}

void loop()
{
        ;
}