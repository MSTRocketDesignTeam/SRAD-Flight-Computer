/*
This abstract class is an interface for the hardware specific SPI SPI peripheral.
All public facing fuctions should be declared here. Specific hardware 
implementation details will be done in the class derived from this base class.
*/

#ifndef SPIINTERFACE_H
#define SPIINTERFACE_H

#include <stdint.h>

using namespace std;

/* ------------------------------ SPI_INTERFACE ----------------------------- */
class SpiInterface
{
        public: 
                //Desc: Writes a single byte into the 
                virtual void write() = 0;
                virtual void read() = 0; 
};
/* -------------------------------------------------------------------------- */
#endif