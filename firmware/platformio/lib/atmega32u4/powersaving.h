/*
This library is responsible for keeping track of the in use peripherals on the 
Atmega32u4 and shutting down clocks and components when they are not in use 
*/

#ifndef POWERSAVING_H
#define POWERSAVING_H

#include <stdint.h>

using namespace std; 

/* ---------------------------- POWERSAVING_CLASS --------------------------- */
class PowerSaving
{
        public: 
                // Constructor
                PowerSaving(); 

                // Desc: Adds a reference to the PLL reference counter.
                //        If initialially zero count, then PLL is enabled. 
                //        If this function must enable the PLL, it will block 
                //        Until PLL Lock is achieved 
                // Args: id: a process unique 2^n id number 
                // Returns: Nothing 
                void lockPLL(const uint_fast8_t id) volatile;

                // Desc: Removes a specific reference to the PLL reference counter
                //       If reference is zero after removal, will disable the PLL
                // Args: id: a process unique 2^n id number
                // Returns: Nothing 
                void unlockPLL(const uint_fast8_t id) volatile; 

                // Desc: Removes the
        protected: 
                // If the PLL is being utilized, this will be nonzero 
                volatile uint_fast8_t PLL_ref = 0;  
};
/* -------------------------------------------------------------------------- */

// Global Class Declaration 
// ------------------------
extern PowerSaving Pwr; 
// ------------------------
#endif