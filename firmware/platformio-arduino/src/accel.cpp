#include "accel.h"
#include "board.h"
#include "SparkFun_KX13X.h"

void Accelerometer::init()
{
        kxAccel.begin(PIN::HG_ACCEL_CS); // initialize accelerometer
        kxAccel.enableAccel(false); // disable so that settings can be changed
        kxAccel.setRange(SFE_KX134_RANGE64G); // 64G range 
        kxAccel.enableDataEngine(); // allows for data ready bit to be set
        kxAccel.setOutputDataRate(7); // pg 26., 100hz 
        kxAccel.enableSampleBuffer(false); // want the read sample to always be new 
        kxAccel.enableAccel(true); // settings done, enable accelerometer
        return; 
}

void Accelerometer::read()
{
        while (!(kxAccel.dataReady())) { ; }
        kxAccel.getRawAccelRegisterData(&kxData);
        return; 
}

// with +/- 64G range, 0.001953125 g / LSB 

int16_t Accelerometer::getX()
{
        return kxData.xData;
}

int16_t Accelerometer::getY()
{
        return kxData.yData;
}

int16_t Accelerometer::getZ()
{
        return kxData.zData; 
}

Accelerometer accel; 