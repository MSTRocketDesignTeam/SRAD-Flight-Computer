// Author: Seth Sievers
// Date: 5/2/24
// Desc: This class will handle the storage, buffering, and writing to FRAM of collected data
//      It will also check stored data for launch conditions 

#include "storage.h"
#include <Arduino.h>
#include "datapkt.h"

/* ------------------------------ STORAGE CLASS ----------------------------- */
dataPkt * Storage::getBufIRef(const uint8_t i)
{
        return &(buf[i]); 
}

uint8_t Storage::numInFIFO()
{
        // Calculate the number of stored elements using modulo to account for rollover
        //! Note, this will benefit from array length that is a power of 2 for speedup
        return (((SRAD_STORAGE_BUF_LENGTH - buf_start_i) + buf_stop_i) % SRAD_STORAGE_BUF_LENGTH);
}

void Storage::bufEnqueue(const dataPkt &data)
{
        // buf_stop_i is where the next element should be placed
        buf[buf_stop_i] = data;

        //! need to compute averages and magnitudes 

        // increment the stop_i accounting for rollover
        buf_stop_i = ((buf_stop_i + 1) % SRAD_STORAGE_BUF_LENGTH);

        // if data was overwritten, must move start ptr
        if (buf_start_i == buf_stop_i) {
                buf_start_i = ((buf_start_i + 1) % SRAD_STORAGE_BUF_LENGTH); 
        }

        return; 
}

dataPkt * Storage::bufDequeue()
{
        // Check that there is something to return from the buffer 
        dataPkt * temp_ptr = NULL;
        if (numInFIFO() > 0) {
                // save ref for return
                temp_ptr = &(buf[buf_start_i]); 

                // increment the index, accounting for rollover
                buf_start_i = ((buf_start_i + 1) % SRAD_STORAGE_BUF_LENGTH); 
        }
        return temp_ptr;
}
/* -------------------------------------------------------------------------- */