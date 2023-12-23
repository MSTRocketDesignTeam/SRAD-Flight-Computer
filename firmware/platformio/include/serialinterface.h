/*
This abstract class is an interface for the 'serial' communication that occurs
with a connected PC. This could be implemented as a uart on the uC or in the case
of the Atmega32u4 this will be a virtual com port using the native USB capability.

The specific implementation will be derived from this base class but the functions 
here will be guaranteed to be implemented
*/

#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include <stdint.h>

using namespace std; 

/* ---------------------------- SERIAL_INTERFACE ---------------------------- */
class SerialInterface 
{
        public:
                // Desc: 
                virtual uint_fast8_t read() = 0; 

                virtual uint_fast8_t write(const uint_fast8_t data) = 0; 
};
/* -------------------------------------------------------------------------- */
#endif