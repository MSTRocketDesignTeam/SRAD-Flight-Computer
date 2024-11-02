#include <Arduino.h>
#include <MS5611_SPI.h> // Barometer Library 
#include <Adafruit_FRAM_SPI.h> // FRAM Library
#include <SparkFun_KX13X.h> // High G Accelerometer Library
#include <LoraSx1262.h> // SX1262 LORA Module Library 

#include "board.h"
#include "storage.h"
#include "gpio.h"

/* ---------------------------------- SETUP --------------------------------- */
void setup() 
{
        // Intialize the board and various libraries 
        boardInit(); 

        // Storage Testing Code
        storage.init();
        storage.eraseAll(); 
        gpioInit(PIN::LED_B, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // blue on 
        while (storage.getState() != Storage::FULL)
        {
                static uint32_t i = 0; 
                // write various packets to attempt to read back after
                storage.writeAccel(i, 0, (static_cast<uint16_t>(1) << 15)/2, (static_cast<uint16_t>(1) << 15)); 
                storage.writeGyro(i, (static_cast<uint16_t>(1) << 15), (static_cast<uint16_t>(1) << 15)/2, 0); 
                storage.writePressure(i, 0); 
                storage.writePressure(i, (static_cast<uint32_t>(1) << 31));
                i++; 
        }
        Serial.print(F("Storage State: ")); 
        switch (storage.getState()) {
                case (Storage::CONTAINS_FLIGHT):
                        Serial.println(F("CONTAINS FLIGHT"));
                        break; 
                case (Storage::EMPTY):
                        Serial.println(F("EMPTY"));
                        break;
                case (Storage::WRITABLE):
                        Serial.println(F("WRITABLE"));
                        break; 
                case (Storage::FULL):
                        Serial.println(F("FULL"));
                        break;
                case (Storage::ERROR):
                        Serial.println(F("ERROR"));
                        break;  
        }
        gpioSet(PIN::LED_B, PIN_STATE::HIGH_S); // blue off
        delay(5000); 

        gpioInit(PIN::LED_R, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // red on 
        // dump the FRAM to the serial terminal 
        storage.printFRAM(); 
        gpioSet(PIN::LED_R, PIN_STATE::HIGH_S); // red off
        delay(5000); 

        gpioInit(PIN::LED_G, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // green on -> Done 

        while (true) { ; }
        return; 
}
/* -------------------------------------------------------------------------- */

/* ---------------------------------- LOOP ---------------------------------- */
void loop() 
{
        return; 
}
/* -------------------------------------------------------------------------- */
