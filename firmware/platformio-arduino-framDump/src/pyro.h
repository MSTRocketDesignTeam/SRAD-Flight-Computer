// Author: Seth Sievers
// Date: 5/3/24
// Desc: A wrapper for enabling and disabling the pyro channels on the SRAD Board


// Pyro Channels -- (ACTIVE HIGH) ----
// CH1 Fire: PB0 
// CH1 Detect: PB4
// CH2 Fire: PD5
// CH2 Detect: PB6
// -----------------------------------

#ifndef PYRO_H
#define PYRO_H

#include <Arduino.h>

/* ---------------------------------- PYRO ---------------------------------- */
void pyro_init(); 

void pyro_ch1(uint8_t state); 

void pyro_ch2(uint8_t state); 
/* -------------------------------------------------------------------------- */
#endif