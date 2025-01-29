/*
Library to periodically read sensors, filter results, and set deployment status flags 
*/

#ifndef FILTERING_H
#define FILTERING_H 

#include <stdint.h>

#define FILTER_NUM_AVERAGE (8)
#define FILTER_SAMPLE_RATE_MS (20) 

class Filter 
{
        public: 
                Filter(); 

                // Desc: Initializes member variables and sensors when called
                // Args: None
                // Returns: None 
                void init(); 

                // Desc: Nonblocking loop, samples every sample interval and updates values 
                // Args: None
                // Returns: Nothing
                void sample(); 

        private: 
                // Acceleration Buffers
                uint32_t xAccelSum, yAccelSum, zAccelSum; 
                uint16_t xAccelArr[FILTER_NUM_AVERAGE];
                uint16_t yAccelArr[FILTER_NUM_AVERAGE];
                uint16_t zAccelArr[FILTER_NUM_AVERAGE];

                // Gyro Buffers 
                uint32_t xRotSum, yRotSum, zRotSum; 
                uint16_t xRotArr[FILTER_NUM_AVERAGE];
                uint16_t yRotArr[FILTER_NUM_AVERAGE]; 
                uint16_t zRotArr[FILTER_NUM_AVERAGE]; 

                // Pressure Buffers
                uint64_t pressureSum; 
                uint32_t pressureArr[FILTER_NUM_AVERAGE]; 

                // Time used for sample function
                uint32_t lastSampleTime = 0;

                void zeroArr(void * arrPtr, const uint8_t dataSize, const uint8_t numElements); 
}; 

#endif