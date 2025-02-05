#include "Arduino.h"
#include "filtering.h"



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
        
        // Zero Avg Arrays 
        zeroArr(xAccelArr, sizeof(xAccelArr), FILTER_NUM_AVERAGE);
}

void Filter::sample()
{
    if ((millis() - lastSampleTime) >= FILTER_SAMPLE_RATE_MS)
    {
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