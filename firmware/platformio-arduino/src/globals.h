// Author: Seth Sievers
// Date: 4/20/24
// Desc: This cursed file is so that I can declare global variables and include them
// in other files. This is not the best practice but without rewriting premade libraries
// (or excessive pass by ref) this is the easiest. 

#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "timer.h"
#include "storage.h"
#include <Adafruit_FRAM_SPI.h> // FRAM Library
#include <SparkFun_KX13X.h> // High G Accelerometer Library
#include <LoraSx1262.h> // SX1262 LORA Module Library 
#include <MS5611_SPI.h> // Barometer Library 

// TIMERS --------------
extern Timer doComCheck; 
extern Timer readSensors; 
// ---------------------

// GLOBAL Vars ---------
#define SENSOR_READ_INTERVAL 100
Storage sensorStorage; 
// ---------------------

// SPI -----------------
// SCK: PB1 (D15)
// MOSI: PB2 (D16)
// MISO: PB3 (D14)
// Baro. CS: PF5 (D20)
// LORA  CS: PF4 (D21)
// FRAM: CS: PF0 (D23) HOLD: PF1 (D22)
#define SPI_SCK_D 15
#define SPI_MOSI_D 16
#define SPI_MISO_D 14
#define BARO_CS_D 20
#define LORA_CS_D 21
#define FRAM_CS_D 23
#define FRAM_HOLD_P PF1 
// ----------------------

// Sensors --------------
extern Adafruit_FRAM_SPI fram; // Fram init
extern SparkFun_KX134_SPI kxAccel; // Kx134 Accelerometer
extern MS5611_SPI ms5611; // Barometer
extern LoraSx1262 radio; // Radio 
// ----------------------

#endif