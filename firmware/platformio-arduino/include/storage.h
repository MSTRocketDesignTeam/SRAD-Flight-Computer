// Date: 10/18/24
// Desc: This class will handle the storage abstraction for the FRAM chip to hold 
//      flight data. 

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <stdint.h>
#include <Adafruit_FRAM_SPI.h>
#include "board.h"

/* ------------------------------ STORAGE_CLASS ----------------------------- */
class Storage 
{
        public: 
                // Constructor 
                Storage();

                // Desc: standalone init function to enforce execution order
                // Args: None
                // Returns: 0 if successful, 1 if error
                uint8_t init(); 

                // Desc: Erase the entirety of the data on the FRAM (Set to 0x00)
                // Args: None
                // Returns: 0 if successful, 1 if error
                uint8_t eraseAll(); 

                // enum to define the state of the Storage system 
                enum STORAGE_STATE : uint8_t 
                {
                        CONTAINS_FLIGHT = 0,
                        EMPTY = 1,
                        WRITABLE = 2, 
                        FULL = 3, 
                        ERROR = 4
                };

                // Desc: returns the state of the storage system
                // Args: None 
                // Returns: STORAGE_STATE enum 
                STORAGE_STATE getState(); 

                

        private: 
                // total number of words in FRAM 
                const uint32_t FRAM_WORD_SIZE = 262144; 
                
                // Adafruit FRAM object 
                Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(PIN::FRAM_CS); 

                // store the state of the FRAM subsystem
                STORAGE_STATE state = CONTAINS_FLIGHT; // prevent rewriting unless verified
}; 

// global declaration for storage class singletons
extern Storage storage; 
/* -------------------------------------------------------------------------- */
#endif