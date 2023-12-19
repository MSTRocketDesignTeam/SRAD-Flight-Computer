#include "time.h"
#include <stdint.h> 
#include <avr/io.h>
#include <avr/interrupt.h>

using namespace std; 

/* ------------------------ TIMECLASS_IMPLEMENTATION ------------------------ */
TimeClass::TimeClass() : milliseconds(0) //initialization list 
{
        // Configure Atmega32u4 8-bit Timer 0 for 1ms interrupts 
        /*
        Datasheet: pg. 104 

        (1 ms) * (16*10^6 CP / 1s) * (1 count / 64 CP) = 250 counts -> OCCR0A = 249
        
        Prescaler: 64x 
        OCCR0A: 249 counts 
        Mode: CTC
        OCCR0A Interrupt Enabled
        */

        OCR0A = 249; // Cause OCF to be set every 1ms 
        TIFR0 = (1 << OCF0A); // Make sure OCF is cleared
        TIMSK0 = (1 << OCIE0A); // Enable OCCR0A interrupt 
        TCNT0 = 0; // Reset timer value 
        TCCR0A = (1 << WGM01); // No output pin mode, CTC mode
        TCCR0B = (1 << CS01) | (1 << CS00); // CTC mode, 64x prescaler 

}

uint_fast32_t TimeClass::millis()
{
        return 0; 
}

void TimeClass::delayMs(const uint_fast16_t ms) const
{
        while (ms != 0) {;}
        return; 
}

inline void TimeClass::ISR_timeIncrement() volatile
{
        milliseconds++; 
}
/* -------------------------------------------------------------------------- */

/* ------------------------ INTERRUPT_SERVICE_ROUTINE ----------------------- */
ISR(TIMER0_COMPA_vect)
{
        Time.ISR_timeIncrement(); 
}
/* -------------------------------------------------------------------------- */

// Create Global TimeClass Object
//---------------------------
TimeClass Time; 
//---------------------------

