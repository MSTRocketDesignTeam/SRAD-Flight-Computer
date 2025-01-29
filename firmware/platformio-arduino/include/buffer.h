/*
A very basic fifo buffer implementation
*/

#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

/* --------------------------------- BUFFER --------------------------------- */
template <typename T, uint8_t numElements> 
class Buf 
{
        public:
                Buf(const T defaultVal)
                {
                        for (uint8_t i = 0; i < numElements; i++)
                        {
                                bufArr[i] = defaultVal; 
                        }
                        return; 
                }

        private:
                T bufArr[numElements]; 


}; 

/* -------------------------------------------------------------------------- */

#endif