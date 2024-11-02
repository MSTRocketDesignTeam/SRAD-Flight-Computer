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

                // enums to define the different components for packet types
                // must be or'd together to form packet header 
                // PCKT HEADER = <Preamble[7:6]> | <ID[5:1]> | <Parity[0]>
                //! Undefined bits should be set as 0
                enum PKT_PREAMBLE : uint8_t //[7:6]
                {
                        NORMAL = 0b01000000
                };
                enum PKT_ID : uint8_t //[5:1]
                {
                        ACCEL_ONLY = (1 << 4),
                        GYRO_ONLY = (1 << 3),
                        ACCEL_AND_GYRO = (1 << 2),
                        ACCEL_GYRO_PRESSURE = (1 << 1),
                        PRESSURE_ONLY = ((1 << 1) | (1 << 2)),
                        EVENT = ((1 << 1) | (1 << 3))
                };

                struct __attribute__((packed)) AccelPkt 
                {
                        uint8_t header; 
                        uint32_t time_ms; 
                        uint16_t xAccel;
                        uint16_t yAccel; 
                        uint16_t zAccel; 
                };

                struct __attribute__((packed)) GyroPkt 
                {
                        uint8_t header; 
                        uint32_t time_ms; 
                        uint16_t xRot;
                        uint16_t yRot; 
                        uint16_t zRot; 
                };

                struct __attribute__((packed)) PressurePkt 
                {
                        uint8_t header; 
                        uint32_t time_ms; 
                        uint32_t pressure; 
                };

                struct __attribute__((packed)) AccelGyroPkt 
                {
                        uint8_t header; 
                        uint32_t time_ms; 
                        uint16_t xAccel;
                        uint16_t yAccel; 
                        uint16_t zAccel; 
                        uint16_t xRot;
                        uint16_t yRot; 
                        uint16_t zRot; 
                };

                struct __attribute__((packed)) AccelGyroPressurePkt 
                {
                        uint8_t header; 
                        uint32_t time_ms; 
                        uint16_t xAccel;
                        uint16_t yAccel; 
                        uint16_t zAccel; 
                        uint16_t xRot;
                        uint16_t yRot; 
                        uint16_t zRot; 
                        uint32_t pressure;
                };

                // Desc: Write an acceleration Packet to the FRAM
                // Returns: 0: success, 1: error 
                uint8_t writeAccel(const uint32_t time, const uint16_t xAccel, 
                                const uint16_t yAccel, const uint16_t zAccel); 

                uint8_t writeGyro(const uint32_t time, const uint16_t xRot, 
                                const uint16_t yRot, const uint16_t zRot);

                uint8_t writePressure(const uint32_t time, const uint32_t pressure);

                uint8_t writeAccelGyro(const uint32_t time, const uint16_t xAccel, 
                                const uint16_t yAccel, const uint16_t zAccel,
                                const uint16_t xRot, const uint16_t yRot, 
                                const uint16_t zRot);

                uint8_t writeAccelGyroPressure(const uint32_t time, const uint16_t xAccel, 
                                const uint16_t yAccel, const uint16_t zAccel,
                                const uint16_t xRot, const uint16_t yRot, 
                                const uint16_t zRot, const uint32_t pressure);

                // Desc: prints the contents of the FRAM to the Serial port (does not decode packets)
                // Returns: 0: success, 1: error 
                uint8_t printFRAM();



        private: 
                // total number of words in FRAM 
                const uint32_t FRAM_WORD_SIZE = 262144; 

                // initialize to start of FRAM
                uint32_t currentFramAddr = 0;
                
                // Adafruit FRAM object 
                Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(PIN::FRAM_CS); 

                // store the state of the FRAM subsystem
                STORAGE_STATE state = CONTAINS_FLIGHT; // prevent rewriting unless verified

                // is single bit parity enabled? This may have some slowdown 
                const uint8_t isParityEnabled = false; 

                // Desc: Determine single bit parity of data of size <size>
                // Args:
                //      data: void pointer representing start of data block
                //      size: how many total bytes to read starting from data
                // Returns: 0 or 1, (bit that will make number of 1's in data even)
                uint8_t computeParity(void * const data, const uint8_t size);

                uint8_t writePkt(void * const data, const uint8_t size);

}; 

// global declaration for storage class singletons
extern Storage storage; 
/* -------------------------------------------------------------------------- */
#endif