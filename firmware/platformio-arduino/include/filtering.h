/*
Library to periodically read sensors, filter results, and set deployment status flags 
*/

#ifndef FILTERING_H
#define FILTERING_H 

#include <stdint.h>
#include "buffer.h"

#define FILTER_NUM_AVERAGE (8)
#define FILTER_SAMPLE_RATE_MS (20) 

class Filter 
{
        public: 
                Filter() { ; }

                // Desc: Initializes member variables and sensors when called
                // Args: None
                // Returns: None 
                void init(); 

                // Desc: Nonblocking loop, samples every sample interval and updates values 
                // Args: None
                // Returns: Nothing
                void sample(); 

                // Desc: Returns the average of values stored in buffer
                // Args: None
                // Returns: float average
                // Note: This function will be relatively slow 
                float getXAccelAvg();
                float getYAccelAvg();
                float getZAccelAvg();
                float getPressureAvg(); 

                enum class ROCKET_STATE : uint8_t { LAUNCH_WAIT = 0, BOOST, APOGEE, PARACHUTE, LANDED}; 


        private: 
                // Acceleration Buffers
                uint32_t xAccelSum, yAccelSum, zAccelSum; 
                Buf<int16_t, FILTER_NUM_AVERAGE> xAccelBuf; 
                Buf<int16_t, FILTER_NUM_AVERAGE> yAccelBuf; 
                Buf<int16_t, FILTER_NUM_AVERAGE> zAccelBuf; 

                // Gyro Buffers 
                uint32_t xRotSum, yRotSum, zRotSum; 
                // Buf<int16_t, FILTER_NUM_AVERAGE> xRotBuf; 
                // Buf<int16_t, FILTER_NUM_AVERAGE> yRotBuf; 
                // Buf<int16_t, FILTER_NUM_AVERAGE> zRotBuf; 

                // Pressure Buffers
                uint64_t pressureSum; 
                Buf<uint32_t, FILTER_NUM_AVERAGE> pressureBuf; 

                // Time used for sample function
                uint32_t lastSampleTime = 0;

                void checkFlightState(); 
}; 

extern Filter filter; 

#endif