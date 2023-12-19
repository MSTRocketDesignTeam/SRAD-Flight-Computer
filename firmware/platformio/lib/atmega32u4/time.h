#ifndef TIME_H
#define TIME_H

#include "timei.h"
#include <stdint.h>

using namespace std; 

/* ------------------------------- TIME_CLASS ------------------------------- */
class Time : public TimeI
{
        public: 
                // Override Functions from Abstract Class
                uint_fast32_t millis() override; 
                void delayMs(const uint_fast16_t ms) override; 
};
/* -------------------------------------------------------------------------- */
#endif
