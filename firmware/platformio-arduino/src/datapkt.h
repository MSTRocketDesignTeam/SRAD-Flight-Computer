// Author: Seth Sievers
// Date: 5/2/24
// Desc: This struct will be used to store the sensor data in the launch detect buffer

#ifndef DATAPKT_H
#define DATAPKT_H

#include <Arduino.h>

/* ------------------------------- DATA PACKET ------------------------------ */
struct dataPkt 
{
        int16_t xAccel;
        int16_t yAccel;
        int16_t zAccel; 
        uint16_t pressure; 
        uint16_t time; // each value is 10ms
};
/* -------------------------------------------------------------------------- */
#endif