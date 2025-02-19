#include "Arduino.h"
#include "filtering.h"
#include "buffer.h"
#include "baro.h"



void Filter::init()
{
        // Zero Sum variables 
        xAccelSum = 0; 
        yAccelSum = 0; 
        zAccelSum = 0;
        xRotSum = 0; 
        yRotSum = 0; 
        zRotSum = 0;
        pressureSum = 0;
        
}

void Filter::sample()
{
    if ((millis() - lastSampleTime) >= FILTER_SAMPLE_RATE_MS)
    {
        // remove oldest pressure reading and add a new sample
         
        if (pressureBuf.getFreeElements() == 0) {
                const uint32_t * temp_ptr = pressureBuf.dequeue(); 
                pressureSum -= (*temp_ptr); 
        }
        if (pressureBuf.getFreeElements() >= 1) {
                uint32_t * const temp_ptr = pressureBuf.enqueue(); 
                (*temp_ptr) = baro.readPressure(); 
                pressureSum += (*temp_ptr); 
        }

        //! Add acceleration and gyro sampling here! 2/18/25



        // remove oldest accelerometer reading and add a new sample
        

        lastSampleTime = millis(); 
        
    }
}

void Filter::zeroArr(void * arrPtr, const uint8_t dataSize, const uint8_t numElements)
{
        for (uint16_t i = 0; i < (dataSize * numElements); i++)
        {
                *(reinterpret_cast<uint8_t *>(arrPtr) + i) = 0; 
        }
        return; 
}