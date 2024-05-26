// Author: Seth Sievers
// Date: 4/21/24
// Desc: Implementation file for the timer wrapper class

#include "timer.h"
#include <Arduino.h>

/* ----------------------- TIMER_CLASS_IMPLEMENTATION ----------------------- */
Timer::Timer(uint16_t interval_) : interval(interval_) { ; }

Timer::operator bool() {
        if ((millis() - lastTime) > interval) {
                lastTime = millis(); 
                return true; 
        }
        return false; 
}

/* -------------------------------------------------------------------------- */