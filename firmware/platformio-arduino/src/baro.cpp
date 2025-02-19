//This file contains the abstraction for reading the barometer
//It uses the MS5611 driver library

#include "MS5611_SPI.h"
#include "baro.h"

Barometer::Barometer()
{
    return;
}

void Barometer::baroInit()
{
    baro = MS5611_SPI(BARO_CS);
    baro.begin();
    baro.setOversampling(OSR_STANDARD);
}

uint32_t Barometer::readPressure()
{
    // 50,000 ft -> 0.11 bar 
    // multiply by 65536 then truncate integer -> 0.000015259 bar resolution 
    // fixed point pressure range: [0, 65536] (max of barometer is 1.2 bar)
    baro.read();
    float pressure = baro.getPressure();
    return static_cast<uint32_t>(pressure * (static_cast<uint32_t>(1) << 16));
}