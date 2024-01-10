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
                // Desc: Reads a single byte from the receive buffer (does not check if bytes available)
                // Args: None
                // Returns: A data byte 
                virtual uint_fast8_t read() = 0; 

                // Desc: Reads 'len' number of bytes into pointer struct starting at data's address 
                //      (Does not check if bytes are available before reading)
                // Args: data: void ptr, len: how many bytes to read 
                // Returns: Nothing 
                virtual void readBytes(void * const data, const uint8_t len) = 0; 

                // Desc: Writes a singular byte into the send buffer (does not check if room in send buffer)
                //      (The buffer is only sent if it fills or flushTX() is called)
                // Args: A byte of data to add to buffer
                // Returns: None
                virtual void write(const uint8_t data) = 0; 

                // Desc: Writes 'len' number of bytes into the send buffer starting at data's address
                //      (Does not check if there is room for the bytes) 
                //      (The buffer is only sent if it fills or flushTX() is called)
                // Args: data: void ptr address, len: how many bytes to write 
                // Returns: Nothing 
                virtual void writeBytes(const void * const data, const uint8_t len) = 0; 

                // Desc: Sends any remaining bytes in the send buffer
                // Args: None
                // Returns: Nothing 
                virtual void flushTX() = 0; 

                // Desc: Discards any bytes stored in the receive buffer 
                // Args: None
                // Returns: Nothing
                virtual void flushRX() = 0; 

                //TODO: print function
};
/* -------------------------------------------------------------------------- */
#endif