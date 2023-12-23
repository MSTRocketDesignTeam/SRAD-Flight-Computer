/*
This is the hardware implementation of the 'serial' interface used to communicate
with a connected PC. This implemenetation for the Atmega32u4 uses the native USB port
to create a virtual com port. This may very by processor. 
*/

#ifndef CDCSERIAL_h
#define CDCSERIAL_h

#include "serialinterface.h"

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
};
/* -------------------------------------------------------------------------- */
#endif