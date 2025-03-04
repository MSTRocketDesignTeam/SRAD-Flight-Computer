#include <Arduino.h>
#include <MS5611_SPI.h> // Barometer Library 
#include <Adafruit_FRAM_SPI.h> // FRAM Library
#include <SparkFun_KX13X.h> // High G Accelerometer Library
#include <LoraSx1262.h> // SX1262 LORA Module Library 

#include "board.h"
#include "storage.h"
#include "gpio.h"
#include "filtering.h"

/* ---------------------------------- SETUP --------------------------------- */
void setup() 
{
        // Intialize the board and various libraries 
        boardInit(); 
        delay(1); // make sure everything is initialized 

        // enable storage
        storage.init();

        // init the sum variables 
        filter.init(); 


        // gpioInit(PIN::LED_R, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // red on
        // while (!Serial) { ; } // wait for pc connection 
        // gpioSet(PIN::LED_R, PIN_STATE::HIGH_S); // red off
        // delay(10000); 

        // // Storage Testing Code
        // storage.init();
        // storage.eraseAll(); //! Need to properly have the code set the storage state on writes/reads, done??
        // gpioInit(PIN::LED_B, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // blue on 
        // Serial.print(F("Before Storage State: ")); 
        // switch (storage.getState()) {
        //         case (Storage::CONTAINS_FLIGHT):
        //                 Serial.println(F("CONTAINS FLIGHT"));
        //                 break; 
        //         case (Storage::EMPTY):
        //                 Serial.println(F("EMPTY"));
        //                 break;
        //         case (Storage::WRITABLE):
        //                 Serial.println(F("WRITABLE"));
        //                 break; 
        //         case (Storage::FULL):
        //                 Serial.println(F("FULL"));
        //                 break;  
        // }
        // while (storage.getState() != Storage::FULL)
        // {
        //         static uint32_t i = 0; 
        //         // write various packets to attempt to read back after
        //         storage.writeAccel(i, 0, (static_cast<uint16_t>(1) << 15)/2, (static_cast<uint16_t>(1) << 15)); 
        //         storage.writeGyro(i, (static_cast<uint16_t>(1) << 15), (static_cast<uint16_t>(1) << 15)/2, 0); 
        //         storage.writePressure(i, 0); 
        //         storage.writePressure(i, (static_cast<uint32_t>(1) << 31));
        //         i++; 
        // }
        // Serial.print(F("After Storage State: ")); 
        // switch (storage.getState()) {
        //         case (Storage::CONTAINS_FLIGHT):
        //                 Serial.println(F("CONTAINS FLIGHT"));
        //                 break; 
        //         case (Storage::EMPTY):
        //                 Serial.println(F("EMPTY"));
        //                 break;
        //         case (Storage::WRITABLE):
        //                 Serial.println(F("WRITABLE"));
        //                 break; 
        //         case (Storage::FULL):
        //                 Serial.println(F("FULL"));
        //                 break;
        // }
        // gpioSet(PIN::LED_B, PIN_STATE::HIGH_S); // blue off
        // delay(5000); 

        // gpioInit(PIN::LED_R, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // red on 
        // // dump the FRAM to the serial terminal 
        // storage.printFRAM(); 
        // gpioSet(PIN::LED_R, PIN_STATE::HIGH_S); // red off
        // delay(5000); 

        // gpioInit(PIN::LED_G, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // green on -> Done 

        // while (true) { ; }
        return; 
}
/* -------------------------------------------------------------------------- */

/* ---------------------------------- LOOP ---------------------------------- */
void loop() 
{
        // SERIAL COMMANDS ------------------------------------------
        // 'S' - Status: Altimeter will print its current state 
        // 'E' - Erase: Altimeter will erase its internal storage 
        // 'R' - Read: Prints stored data to terminal in csv format 
        // Using very simple command structure for handling serial data 
        // Single character command simplifies the command identificiation
        uint8_t r = 0; 
        if (Serial) { // if this is not initialized then print statements will hang
                // when there is no serial data available, read will return -1 = 255
                r = Serial.read(); 

                if (r != 0xFF) { // -1 in binary is 0xFF
                        // Communication has been received 

                        // echo so that putty does not require additional echo settings
                        Serial.print(r); 

                        // Switch case to handle commands 
                        switch (r)
                        {
                                case ('S'):
                                        // STATUS -------------
                                        Serial.println(F("--- SRAD_ALTIMETER ---"));
                                        Serial.print(F("STORAGE_STATE: "));
                                        const uint8_t storage_state = storage.getState(); 
                                        switch (storage_state)
                                        {
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
                                                default:
                                                        Serial.print(F("INVALID, "));
                                                        Serial.print(storage_state, );
                                        };
                                        break; 
                                default:
                                        break; 
                        };
                }
        }
        // ----------------------------------------------------------
        return; 
}
/* -------------------------------------------------------------------------- */
