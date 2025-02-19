#ifndef ACCEL_H
#define ACCEL_H

#include "SparkFun_KX13X.h"
#include <Arduino.h>
#include <stdint.h>


class Accelerometer
{
        public:
                Accelerometer() { ; } 

                void init(); 

                // read data from the accelerometer and store in data struct 
                void read(); 

                // convert the accel struct data into 16b int and return 
                int16_t getX();
                int16_t getY();
                int16_t getZ(); 
        private:
                SparkFun_KX134_SPI kxAccel; 
                rawOutputData kxData; 
};

extern Accelerometer accel; 


#endif 