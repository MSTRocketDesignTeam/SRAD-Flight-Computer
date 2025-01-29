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
    baro.read();
    float pressure = baro.getPressure();
    return static_cast<uint32_t>(pressure * (static_cast<uint32_t>(1) << 16));
}