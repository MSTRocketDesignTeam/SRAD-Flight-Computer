/*
This is the hardware implementation of the 'serial' interface used to communicate
with a connected PC. This implemenetation for the Atmega32u4 uses the native USB port
to create a virtual com port. This may very by processor. 
*/

#ifndef CDCSERIAL_h
#define CDCSERIAL_h

#include "serialinterface.h"
#include <avr/io.h>
#include <avr/interrupt.h>

using namespace std; 

/* ------------------------------ SERIAL_CLASS ------------------------------ */
class SerialClass : public SerialInterface
{
        public:
                // Constructor
                SerialClass(); 

                // Overrided functions from interface
                uint_fast8_t read() override; 
                uint_fast8_t write(const uint_fast8_t data) override;

        protected: 

                // Desc: Configures the PLL for generating USB clock 
                // Args: None
                // Returns: None 
                inline void configurePLL(); 

                // Desc: Unfreezes the USB CLK
                // Args: None
                // Returns: None 
                inline void enableUSBCLK();

                // Desc: Freezes the USB CLK for power savings 
                // Args: None 
                // Returns: None 
                inline void disableUSBCLK(); 

                volatile uint_fast8_t state; 

                // Keep track of the USB interface STATE. 
                enum USBState
                {
                        BUS_DEFAULT_STATE = 0, // Completely unconfigured, CPU reset state 
                        
                };

};
/* -------------------------------------------------------------------------- */

// USB ISRs 
// -------------------
ISR(USB_COM_vect); 
ISR(USB_GEN_vect); 
// ------------------


// Allow Global Serial Object to be referenced anywhere
//------------------------
extern SerialClass Serial; 
//------------------------

#endif