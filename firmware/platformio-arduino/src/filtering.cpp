#include "Arduino.h"
#include "filtering.h"
#include "buffer.h"



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
        // read the current pressure 
        uint32_t * uint_ptr = pressureBuf.






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