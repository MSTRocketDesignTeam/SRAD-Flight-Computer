// Author: Seth Sievers
// Date: 5/2/24
// Desc: This class will handle the storage, buffering, and writing to FRAM of collected data
//      It will also check stored data for launch conditions 

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include "datapkt.h"

// PARAMETERS
//// For 10Hz collection and 5s of averaging, 50 needed
#define SRAD_STORAGE_BUF_LENGTH 16
#define SRAD_FRAM_WORD_LENGTH 262144
#define SRAD_LANDED_KEY 0xB5


/* ------------------------------ STORAGE CLASS ----------------------------- */
class Storage
{
        public:
                Storage();

                // Returns a pointer reference to dataPkt in buf
                dataPkt * getBufIRef(const uint8_t i); 

                // read the persistent configuration from the fram 
                void cfgInit(); 

                // Calculate how many elements are in the RAM FIFO 
                uint8_t numInFIFO(); 

                // add a dataPkt to the queue and handle FRAM storage 
                void addSensorData(const dataPkt &data);

                // add data to the FIFO 
                void bufEnqueue(const dataPkt &data); 

                // add a packet to the FRAM
                void framEnqueue(const dataPkt &data); 

                // read a packet from the FRAM 
                // give packet number from start of circular fram fifo 
                dataPkt framRead(const uint8_t i); 

                // mark an element as removed from queue and return ptr to it 
                dataPkt * bufDequeue(); 

                uint8_t inFlight(); // returns true or false


        private:
                // CIRCULAR SAMPLE FIFO 
                dataPkt buf[SRAD_STORAGE_BUF_LENGTH]; // will hold all samples initially
                uint8_t buf_start_i = 0; // start of data in buffer
                uint8_t buf_stop_i = 0; // element after end of data in buffer

                // CIRCULAR FRAM 
                const uint8_t fram_offset = 9; // first 9 bits are reserved for persistent config bits
                uint32_t fram_start_i = fram_offset; // address in fram where data starts
                uint32_t fram_stop_i = fram_offset; // stop of data, next address to write
                uint8_t hasLanded = 0; 

                uint16_t lowestPressure = 60000; 
                uint16_t inFlightPressureThreshold = static_cast<uint16_t>(303.975 * (13107.0 / 230.0));
                uint32_t timeInFlightThreshold = 0; // when the in flight pressure threshold was met

                // Launch Condition Detect 
                //uint8_t accelMagnitude[SRAD_STORAGE_BUF_LENGTH]; 

};
/* -------------------------------------------------------------------------- */
#endif