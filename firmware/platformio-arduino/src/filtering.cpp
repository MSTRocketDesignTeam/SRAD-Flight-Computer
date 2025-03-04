#include "Arduino.h"
#include "filtering.h"
#include "buffer.h"
#include "baro.h"
#include "accel.h"



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
                // Read from barometer 
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

                // Read from the Accelerometer
                //! Note: Might need to check if registers contain valid data? 
                accel.read(); 
                // Store X
                if (xAccelBuf.getFreeElements() == 0) {
                        const int16_t * temp_ptr = xAccelBuf.dequeue(); 
                        xAccelSum -= (*temp_ptr); 
                }
                if (xAccelBuf.getFreeElements() >= 1) {
                        int16_t * const temp_ptr = xAccelBuf.enqueue(); 
                        (*temp_ptr) = accel.getX(); 
                        xAccelSum += (*temp_ptr); 
                }
                // Store Y
                if (yAccelBuf.getFreeElements() == 0) {
                        const int16_t * temp_ptr = yAccelBuf.dequeue(); 
                        yAccelSum -= (*temp_ptr); 
                }
                if (yAccelBuf.getFreeElements() >= 1) {
                        int16_t * const temp_ptr = yAccelBuf.enqueue(); 
                        (*temp_ptr) = accel.getY(); 
                        yAccelSum += (*temp_ptr); 
                }
                // Store Z
                if (zAccelBuf.getFreeElements() == 0) {
                        const int16_t * temp_ptr = zAccelBuf.dequeue(); 
                        zAccelSum -= (*temp_ptr); 
                }
                if (zAccelBuf.getFreeElements() >= 1) {
                        int16_t * const temp_ptr = zAccelBuf.enqueue(); 
                        (*temp_ptr) = accel.getZ(); 
                        zAccelSum += (*temp_ptr); 
                }
                lastSampleTime = millis(); 

                checkFlightState(); 
        }
        return; 
}

float Filter::getXAccelAvg()
{
        const uint8_t n = xAccelBuf.getNumElements(); 
        if (n == 0) { return -1.0f; }
        return (xAccelSum / static_cast<float>(n)); 
}

float Filter::getYAccelAvg()
{
        const uint8_t n = yAccelBuf.getNumElements(); 
        if (n == 0) { return -1.0f; }
        return (yAccelSum / static_cast<float>(n)); 
}

float Filter::getZAccelAvg()
{       
        const uint8_t n = zAccelBuf.getNumElements(); 
        if (n == 0) { return -1.0f; }
        return (zAccelSum / static_cast<float>(n)); 
}

float Filter::getPressureAvg()
{
        const uint8_t n = pressureBuf.getNumElements();
        if (n == 0) { return -1.0f; }
        return (pressureSum / static_cast<float>(n)); 
}

void Filter::checkFlightState()
{
        return; 
}

Filter filter;