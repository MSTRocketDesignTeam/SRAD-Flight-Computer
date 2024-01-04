#ifndef LIGHTDEBUG_H
#define LIGHTDEBUG_H

#include <stdint.h>

using namespace std; 

void configureLEDs();
void redOn();
void redOff(); 
void yellowOn();
void yellowOff(); 
void blueOn();
void blueOff(); 
void greenOn();
void greenOff(); 
void print(const uint8_t num); 
#endif