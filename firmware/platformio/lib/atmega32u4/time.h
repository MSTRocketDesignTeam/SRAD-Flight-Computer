/*
This class is the hardware specific implementation for keeping track of time
on the Atmega32u4.
*/

#ifndef TIMECLASS_H
#define TIMECLASS_H

#include "timeinterface.h"
#include <stdint.h>

using namespace std; 

/* ------------------------------- TIME_CLASS ------------------------------- */
class TimeClass : public TimeInterface
{
        public: 
                // Constructor
                TimeClass();

                // Override Functions from Abstract Class
                uint_fast32_t millis() const override; 
                void delayMs(const uint_fast16_t ms) const override; 

                // Desc: Interrupt handler that will run every 1ms, keeping time
                // Args: None
                // Returns: None 
                inline void ISR_timeIncrement() volatile;

        protected:
                volatile uint_fast32_t milliseconds;
};
/* -------------------------------------------------------------------------- */

// Allow Global Time Object to be referenced anywhere
//------------------------
extern TimeClass Time; 
//------------------------
#endif
