#include "Arduino.h"
#include "filtering.h"
#include "buffer.h"
#include "baro.h"
#include "accel.h"
#include "gpio.h"



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
                if (pressureBuf.getFreeElements() == 0) { // if no room
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
                if (xAccelBuf.getFreeElements() == 0) { // if no room
                        const int16_t * temp_ptr = xAccelBuf.dequeue(); 
                        xAccelSum -= (*temp_ptr); 
                }
                if (xAccelBuf.getFreeElements() >= 1) {
                        int16_t * const temp_ptr = xAccelBuf.enqueue(); 
                        (*temp_ptr) = accel.getX(); 
                        xAccelSum += (*temp_ptr); 
                }
                // Store Y
                if (yAccelBuf.getFreeElements() == 0) { // if no room
                        const int16_t * temp_ptr = yAccelBuf.dequeue(); 
                        yAccelSum -= (*temp_ptr); 
                }
                if (yAccelBuf.getFreeElements() >= 1) {
                        int16_t * const temp_ptr = yAccelBuf.enqueue(); 
                        (*temp_ptr) = accel.getY(); 
                        yAccelSum += (*temp_ptr); 
                }
                // Store Z
                if (zAccelBuf.getFreeElements() == 0) { // if no room
                        const int16_t * temp_ptr = zAccelBuf.dequeue(); 
                        zAccelSum -= (*temp_ptr); 
                }
                if (zAccelBuf.getFreeElements() >= 1) {
                        int16_t * const temp_ptr = zAccelBuf.enqueue(); 
                        (*temp_ptr) = accel.getZ(); 
                        zAccelSum += (*temp_ptr); 
                }
                lastSampleTime = millis(); 

                // update flight states 
                checkFlightState(); 
        }
        return; 
}

float Filter::getXAccelAvg()
{
        const uint8_t n = xAccelBuf.getNumElements(); 
        if (n == 0) { return -1.0f; }
        return ((xAccelSum / static_cast<float>(n)) * (1.0f/512.0f)); // conversion to g
}

float Filter::getYAccelAvg()
{
        const uint8_t n = yAccelBuf.getNumElements(); 
        if (n == 0) { return -1.0f; }
        return ((yAccelSum / static_cast<float>(n)) * (1.0f/512.0f)); // conversion to g
}

float Filter::getZAccelAvg()
{
        const uint8_t n = zAccelBuf.getNumElements(); 
        if (n == 0) { return -1.0f; }
        return ((zAccelSum / static_cast<float>(n)) * (1.0f/512.0f)); // conversion to g
}

float Filter::getPressureAvg()
{
        const uint8_t n = pressureBuf.getNumElements();
        if (n == 0) { return -1.0f; }
        return ((pressureSum / static_cast<float>(n)) / 65536.0f); // conversion to milliBar 
}

Filter::ROCKET_STATE Filter::getState()
{
        return flightState; 
}

// Optimize with fixed point in future 
void Filter::checkFlightState()
{
        const float LAUNCH_ACCEL_MAG_GS = 4.0f;
        const float APOGEE_ACCEL_MAG_GS = 1.1f; 
        const float APOGEE_PRESSURE_MB = 933.291f; // use website to calculate pressure threshold accounting for land elevation: https://www.mide.com/air-pressure-at-altitude-calculator
        const float LANDED_GS_MIN = 0.9f; 
        const float LANDED_GS_MAX = 1.1f; 
        const float LANDED_PRESSURE_MAX = 957.5225f; 

        static uint32_t temp32 = 0; 

        float temp = 0.0f; 
        switch (flightState)
        {
                case (ROCKET_STATE::LAUNCH_WAIT):
                        // WAIT FOR LAUNCH CONDITION 
                        
                        // if buffer not mostly full, invalid 
                        if (xAccelBuf.getFreeElements() <= 2) {
                                temp = sqrt(pow(getXAccelAvg(),2) + pow(getYAccelAvg(),2) + pow(getZAccelAvg(),2));
                                // if magnitude greater than threshold, launch detected
                                if (temp > LAUNCH_ACCEL_MAG_GS) {
                                        flightState = ROCKET_STATE::BOOST; 
                                        temp32 = millis(); 
                                }
                        }
                        break; 
                case (ROCKET_STATE::BOOST):
                        // Wait for apogee condition
                        temp = sqrt(pow(getXAccelAvg(),2) + pow(getYAccelAvg(),2) + pow(getZAccelAvg(),2));
                        if ((temp < APOGEE_ACCEL_MAG_GS) && (getPressureAvg() < APOGEE_PRESSURE_MB) && ((millis() - temp32) > 3000)) {
                                temp32 = millis(); 
                                flightState = ROCKET_STATE::APOGEE; 
                        }
                        break;
                case (ROCKET_STATE::APOGEE):
                        // fire pyro for 1s
                        gpioSet(PIN::CH1_FIRE, PIN_STATE::HIGH_S); 
                        if ((millis() - temp32) > 1000) {
                                temp32 = millis(); 
                                flightState = ROCKET_STATE::FALL; 
                                gpioSet(PIN::CH1_FIRE, PIN_STATE::LOW_S);
                        }
                        break; 
                case (ROCKET_STATE::FALL):
                        temp = sqrt(pow(getXAccelAvg(),2) + pow(getYAccelAvg(),2) + pow(getZAccelAvg(),2));
                        if ((temp < LANDED_GS_MAX) && (temp > LANDED_GS_MIN) && (getPressureAvg() < LANDED_PRESSURE_MAX) && ((millis() - temp32) > 4000)) {
                                flightState = ROCKET_STATE::LANDED; 
                                temp32 = millis(); 
                        }
                        break;
                case (ROCKET_STATE::LANDED):
                        break; 
        }
        return; 
}

Filter filter;