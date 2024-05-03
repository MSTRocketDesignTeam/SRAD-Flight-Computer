// Author: Seth Sievers
// Date: 4/20/24
// Desc: Definition of globally declared variables

#include "globals.h"
#include "timer.h"
#include <Adafruit_FRAM_SPI.h> // FRAM Library
#include <SparkFun_KX13X.h> // High G Accelerometer Library
#include <LoraSx1262.h> // SX1262 LORA Module Library 
#include <MS5611_SPI.h> // Barometer Library 

// TIMERS --------------
// Every 250ms we will check the usb bus 
// Timer doComCheck(250); 
// ---------------------

// Sensors --------------
Adafruit_FRAM_SPI fram(FRAM_CS_D); //Fram init
SparkFun_KX134_SPI kxAccel; // Kx134 Accelerometer
MS5611_SPI ms5611(BARO_CS_D); 
LoraSx1262 radio; 
// -----------------------
