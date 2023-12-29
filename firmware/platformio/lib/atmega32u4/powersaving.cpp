#include "powersaving.h"
#include <avr/io.h>

using namespace std; 

/* --------------------- POWERSAVING_IMPLEMENTATION -------------------- */
PowerSaving::PowerSaving()
{
        // PLL should start disabled
        PLLCSR &= ~(1 << PLLE); 
}

void PowerSaving::lockPLL(const uint_fast8_t id) volatile
{
        // This function assumes that the id is valid. Thus if PLL_REF is currently
        // zero it will be enabled 
        if (PLL_ref == 0) {
                // Start the PLL frequency lock process (atmega32u4, pg 40)
                PLLCSR |= (1 << PLLE);

                // Wait for successful lock 
                while (!((1 << PLOCK) & PLLCSR)) { ; }
        }

        // Add the reference to the count 
        PLL_ref |= id; 

        return; 
}

void PowerSaving::unlockPLL(const uint_fast8_t id) volatile
{
        // This function assumes that the id is valid. 
        PLL_ref &= ~id; 

        // Disable PLL if needed 
        if (PLL_ref == 0) {
                PLLCSR &= ~(1 << PLLE); 
        }
        
        return; 
}

/* -------------------------------------------------------------------------- */

// Global Class Definition 
// ------------------------
PowerSaving Pwr; 
// ------------------------