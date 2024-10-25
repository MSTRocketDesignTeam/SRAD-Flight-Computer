#include <Arduino.h>
#include <MS5611_SPI.h> // Barometer Library 
#include <Adafruit_FRAM_SPI.h> // FRAM Library
#include <SparkFun_KX13X.h> // High G Accelerometer Library
#include <LoraSx1262.h> // SX1262 LORA Module Library 

#include "board.h"
#include "storage.h"

/* ---------------------------------- SETUP --------------------------------- */
void setup() 
{
        storage.init(); 
        return; 
}
/* -------------------------------------------------------------------------- */

/* ---------------------------------- LOOP ---------------------------------- */
void loop() 
{
        return; 
}
/* -------------------------------------------------------------------------- */
