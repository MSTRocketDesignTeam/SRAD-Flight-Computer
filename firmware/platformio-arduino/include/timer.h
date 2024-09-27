// Author: Seth Sievers
// Date: 4/21/24
// Desc: A super simple timing wrapper class to allow for persistent updating 
// timers 

#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

/* ------------------------------- TIMER_CLASS ------------------------------ */
class Timer
{
        public: 
                // Arguments: interval, how long between timer activates in ms 
                // Returns: Nothing
                // Desc: Constructor 
                Timer(const uint16_t interval_); 

                // Arguments: None
                // Returns: True if time elapsed, false else
                // Desc: Checks the time condition and updates the last time variable
                explicit operator bool(); 

                // Keeps track of when timer was last activated
                uint32_t lastTime = 0; 

                // How many seconds 
                uint16_t interval;

};


/* -------------------------------------------------------------------------- */
#endif