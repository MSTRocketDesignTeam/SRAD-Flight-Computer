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

uint8_t Storage::inFlight()
{
        return !hasLanded; 
}

void Storage::framEnqueue(const dataPkt &data)
{
        // write the data packet, need an 8bit ptr 
        const uint8_t * write_ptr = reinterpret_cast<const uint8_t *>(&data); 
        fram.writeEnable(true); 
        if ((fram_stop_i+sizeof(dataPkt)) > SRAD_FRAM_WORD_LENGTH) { return; } // crutch, drop packet if issue will arise 
        fram.write(fram_stop_i, write_ptr, sizeof(dataPkt)); // write the correct size to the fram 
        fram.writeEnable(false); 

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
        // determine start address of packet in question 
        // i is zero indexed, must add sizeof datapkt per i and account for wrap around 
        uint8_t pktAddr = (((fram_start_i - fram_offset) + (sizeof(dataPkt) * i)) % (SRAD_FRAM_WORD_LENGTH - fram_offset)) + fram_offset; 

        // read the multibyte data pkt and return it 
        dataPkt tempPkt; 
        uint8_t * readPtr = reinterpret_cast<uint8_t *>(&tempPkt); 
        fram.read(pktAddr, readPtr, sizeof(dataPkt)); 

        return tempPkt;
}

void Storage::cfgInit()
{
        // ADDRESS ALLOCATIONS
        // 0-3: fram_start_i
        // 4-7: fram_stop_i
        // 8: hasLanded
                //// NOTE: if addr=8 reads 0b10100110 (0xA6) then landed state
                // NOTE: if addr=8 reads 0b01011001 (0x59) then unlanded state
                // NOTE: if addr=8 reads other, then erase everything 
        fram.writeEnable(false); // we are reading not writing

        // need pointers to 32bit numbers to byte manipulate them 
        uint8_t * start_i_ptr = reinterpret_cast<uint8_t *>(&fram_start_i);
        uint8_t * stop_i_ptr = reinterpret_cast<uint8_t *>(&fram_stop_i);

        switch (fram.read8(8)) 
        {
                case (SRAD_LANDED_KEY):
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

                        // wipe the fram
                        for (uint32_t i = 9; i < 262144; i++)
                        {
                                fram.write8(i, 0); // wipe all the data storage
                        }
                        fram.write8(8, 0x59); // now valid unlanded state
                        fram.writeEnable(false); 
                        hasLanded = 0; 
                        break; 
        }

        // initialize the pressure in buf so that avg does not get messed up
        for (uint8_t i = 0; i < SRAD_STORAGE_BUF_LENGTH; i++)
        {
                ms5611.read(); 
                buf[i].pressure = static_cast<uint16_t>(ms5611.getPressure() * (13107.0 / 230.0));
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
        // to determine landing condition I will keep track of lowest avg pressure 
        uint32_t pressure_sum = 0; 
        for (uint8_t i = 0; i < SRAD_STORAGE_BUF_LENGTH; i++)
        {
                pressure_sum += buf[i].pressure; 
        }
        uint16_t pressure_avg = pressure_sum / SRAD_STORAGE_BUF_LENGTH;

        // update lowest if valid 
        if (pressure_avg < lowestPressure) {lowestPressure = pressure_avg;}

        // if has landeded update variable
        if (lowestPressure < inFlightPressureThreshold) {
                if (timeInFlightThreshold == 0) {
                        // mark when it was first met
                        timeInFlightThreshold = millis(); 
                }
                // if 10m has elapsed mark has landed
                if ((millis() - timeInFlightThreshold) > (10UL*60UL*1000UL)){
                        hasLanded = 1; 
                        fram.writeEnable(true); 
                        fram.write8(8, SRAD_LANDED_KEY);
                        fram.writeEnable(false); 
                }
        }

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