/*
This abstract class is an interface for the hardware specific "time.h"
implementations. All public facing functions should be declared here. 
Specific hardware implementations will be derived from this base class. 
*/

#ifndef TIMEINTERFACE_H
#define TIMEINTERFACE_H

#include <stdint.h>

using namespace std; 

/* ----------------------------- TIME_INTERFACE ----------------------------- */
class TimeInterface
{
        public: 
                // Desc: Getter for time variable, interrupt safe. 
                // Args: None
                // Returns: 32-bit int specifying global millisecond count
                virtual uint_fast32_t millis() const = 0; 

                // Desc: Blocking delay function 
                // Args: unsigned integer specifying milliseconds [0, 65535]
                // Returns: Nothing
                virtual void delayMs(const uint_fast16_t ms) const = 0; 
};
/* -------------------------------------------------------------------------- */
#endif