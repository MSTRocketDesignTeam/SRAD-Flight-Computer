// Author: Seth Sievers
// Date: 5/2/24
// Desc: This class will handle the storage, buffering, and writing to FRAM of collected data
//      It will also check stored data for launch conditions 

#include "storage.h"
#include <Arduino.h>
#include "datapkt.h"
#include "globals.h"

/* ------------------------------ STORAGE CLASS ----------------------------- */
Storage::Storage() {;}

void Storage::framEnqueue(const dataPkt &data)
{
        // write the data packet, need an 8bit ptr 
        const uint8_t * write_ptr = reinterpret_cast<const uint8_t *>(&data); 
        fram.write(fram_stop_i, write_ptr, sizeof(dataPkt)); // write the correct size to the fram 

        // calculate address of next write, value has an offset, must remove and replace
        fram_stop_i = ((fram_stop_i - fram_offset + sizeof(dataPkt)) % (SRAD_FRAM_WORD_LENGTH - fram_offset)) + fram_offset;
        
        // if stop is the same or greater than start, must move start 
        if ((fram_stop_i + sizeof(dataPkt) - 1) >= fram_start_i){
                //move start to be 1 pkt away from stop
                fram_start_i = (((fram_stop_i - fram_offset) + sizeof(dataPkt)) % (SRAD_FRAM_WORD_LENGTH - fram_offset)) + fram_offset;
        }
}

dataPkt Storage::framRead(const uint8_t i)
{
        return NULL;
}

void Storage::cfgInit()
{
        // ADDRESS ALLOCATIONS
        // 0-3: fram_start_i
        // 4-7: fram_stop_i
        // 8: hasLanded
                // NOTE: if addr=8 reads 0b10100110 (0xA6) then landed state
                // NOTE: if addr=8 reads 0b01011001 (0x59) then unlanded state
                // NOTE: if addr=8 reads other, then erase everything 
        fram.writeEnable(false); // we are reading not writing

        // need pointers to 32bit numbers to byte manipulate them 
        uint8_t * start_i_ptr = reinterpret_cast<uint8_t *>(&fram_start_i);
        uint8_t * stop_i_ptr = reinterpret_cast<uint8_t *>(&fram_stop_i);

        switch (fram.read8(8)) 
        {
                case (0xA6):
                        // HAS ALREADY LANDED
                        hasLanded = 1; 
                        // read the values from fram into ram
                        for (uint8_t i = 0; i < 4; i++) 
                        {
                                *(start_i_ptr+i) = fram.read8(i); //Read start addr
                                *(stop_i_ptr+i) = fram.read8(4+i); //Read stop addr 
                        }
                        break; 
                case (0x59):
                        // UNLANDED STATE 
                        hasLanded = 0; 
                        // if unlanded read from fram into ram 
                        for (uint8_t i = 0; i < 4; i++) 
                        {
                                *(start_i_ptr+i) = fram.read8(i); //Read start addr
                                *(stop_i_ptr+i) = fram.read8(4+i); //Read stop addr 
                        }
                        break; 
                default: 
                        // INVALID STATE
                        // if invalid, write 0 to the values and reset them in ram 
                        fram.writeEnable(true); 
                        for (uint8_t i = 0; i < 4; i++) 
                        {
                                fram.write8(i, 0);
                                fram.write8(4+i, 0);
                        }
                        fram.writeEnable(false); 
                        hasLanded = 0; 
                        break; 
        }
        return; 
}

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