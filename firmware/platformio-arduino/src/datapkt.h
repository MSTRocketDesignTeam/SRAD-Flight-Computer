// Author: Seth Sievers
// Date: 5/2/24
// Desc: This struct will be used to store the sensor data in the launch detect buffer

#ifndef DATAPKT_H
#define DATAPKT_H

#include <Arduino.h>

/* ------------------------------- DATA PACKET ------------------------------ */
struct dataPkt 
{
        uint16_t xAccel;
        uint16_t yAccel;
        uint16_t zAccel; 
};
/* -------------------------------------------------------------------------- */
#endif