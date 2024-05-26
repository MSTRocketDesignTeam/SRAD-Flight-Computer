// Author: Seth Sievers
// Date: 5/3/24
// Desc: A wrapper for enabling and disabling the pyro channels on the SRAD Board

#include "pyro.h"

/* ---------------------------------- PYRO ---------------------------------- */
void pyro_init()
{
        // Disable Pyro Channels -------------
        // Disable CH1 PINS
        PORTB &= ~(1 << PB0); // disable CH1 Fire
        PORTB &= ~(1 << PB4); // disable CH1 Detect
        DDRB |= (1 << PB0); // CH1 Fire is Low Z
        DDRB &= ~(1 << PB4); // CH1 Detect is High Z

        // Disable CH2
        PORTD &= ~(1 << PD5); // Disable CH2 Fire 
        PORTB &= ~(1 << PB6); // Disable CH2 Detect
        DDRD |= (1 << PD5); // CH2 Fire is Low Z
        DDRB &= ~(1 << PB6); // CH2 Detect is High Z
        // -----------------------------------
        return; 
}

void pyro_ch1(uint8_t state)
{
        if (state) {
                // Fire CH1 
                PORTB |= (1 << PB0); 
        } else {
                // Disable CH1 
                PORTB &= ~(1 << PB0);
        }
        return; 
}

void pyro_ch2(uint8_t state)
{
        if (state) {
                // Fire CH2
                PORTD |= (1 << PD5); 
        } else {
                // Disable CH2 
                PORTD &= ~(1 << PD5); 
        }
        return; 
}
/* -------------------------------------------------------------------------- */