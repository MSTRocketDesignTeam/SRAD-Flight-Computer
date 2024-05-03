// Author: Seth Sievers
// Date: 5/2/24
// Desc: This class will handle the storage, buffering, and writing to FRAM of collected data
//      It will also check stored data for launch conditions 

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include "datapkt.h"

// PARAMETERS
// For 10Hz collection and 5s of averaging, 50 needed
#define SRAD_STORAGE_BUF_LENGTH 64


/* ------------------------------ STORAGE CLASS ----------------------------- */
class Storage
{
        public:
                Storage();

                // Returns a pointer reference to dataPkt in buf
                dataPkt * getBufIRef(const uint8_t i); 

                // Calculate how many elements are in the RAM FIFO 
                uint8_t numInFIFO(); 

                // add a dataPkt to the queue and handle FRAM storage 
                void addSensorData(const dataPkt &data);



        private:
                // CIRCULAR SAMPLE FIFO 
                dataPkt buf[SRAD_STORAGE_BUF_LENGTH]; // will hold all samples initially
                uint8_t buf_start_i = 0; // start of data in buffer
                uint8_t buf_stop_i = SRAD_STORAGE_BUF_LENGTH - 1; // element after end of data in buffer
                void bufEnqueue(const dataPkt &data); // add data to the FIFO 
                dataPkt * bufDequeue(); // mark an element as removed from queue and return ptr to it 

                // Launch Condition Detect 
                uint8_t accelMagnitude[SRAD_STORAGE_BUF_LENGTH]; 

};
/* -------------------------------------------------------------------------- */
#endif