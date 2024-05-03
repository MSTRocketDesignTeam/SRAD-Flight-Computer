// Author: Seth Sievers
// Date: 4/20/24
// Desc: Definition of globally declared variables

#include "globals.h"

// TIMERS --------------
// Every 250ms we will check the usb bus 
// Timer doComCheck(250); 
// ---------------------

// Sensors --------------
Adafruit_FRAM_SPI fram(FRAM_CS_D); //Fram init
SparkFun_KX134_SPI kxAccel; // Kx134 Accelerometer
// -----------------------
