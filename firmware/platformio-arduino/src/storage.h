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
#define SRAD_STORAGE_BUF_LENGTH 50


/* ------------------------------ STORAGE CLASS ----------------------------- */
class Storage
{
        public:
                Storage();


        private:
                // CIRCULAR SAMPLE FIFO 
                dataPkt buf[SRAD_STORAGE_BUF_LENGTH]; // will hold all samples initially
                uint8_t buf_start_i = 0; // next empty index 
                uint8_t buf_stop_i = SRAD_STORAGE_BUF_LENGTH - 1; // last written index
};
/* -------------------------------------------------------------------------- */
#endif