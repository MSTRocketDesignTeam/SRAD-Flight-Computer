//This file contains the abstraction for reading the barometer
//It uses the MS5611 driver library

#ifndef BARO_H
#define BARO_H

#include "MS5611_SPI.h"
#include "board.h"


class Barometer
{
    public:
        //Constructor
        Barometer(); 

        // Desc: Initializes the barometer
        // Args: None
        // Returns: Nothing
        void baroInit();

        // Desc: Reads pressure from barometer
        // Args: None
        // Returns: 32 bit value of pressure
        uint32_t readPressure();

    private:
        //MS5611_SPI object
        MS5611_SPI baro = MS5611_SPI(BARO_CS); 
};

extern Barometer baro; 

#endif