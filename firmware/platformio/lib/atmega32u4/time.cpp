#include "time.h"
#include <stdint.h> 

uint_fast32_t Time::millis()
{
        return 0; 
}

void Time::delayMs(const uint_fast16_t ms)
{
        while (ms != 0) {;}
        return; 
}

