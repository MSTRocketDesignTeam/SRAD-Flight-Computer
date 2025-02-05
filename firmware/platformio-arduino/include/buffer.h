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
                Buf(const T &defaultVal)
                {
                        for (uint8_t i = 0; i < numElements; i++)
                        {
                                bufArr[i] = defaultVal; 
                        }
                }

                T * const enqueue()
                {
                        // is space available? 
                        // if (((write_index + 1) % numElements) != read_index)
                        return; 
                }

                const T * const dequeue(); 

                uint8_t getNumElements()
                {
                        // if write == read, no elements in buffer 
                        // if write > read -> (write - read) elements in buffer
                        // if write < read -> ((numElements - read) + write + 1) elements in buffer 
                        if (write_index > read_index) {
                                return (write_index - read_index); 
                        } else if (write_index < read_index) {
                                return ((numElements - read_index) + write_index + 1); 
                        } else {
                                return (0); 
                        }
                }

        private:
                // main buffer array 
                T bufArr[numElements]; 
                
                // start and stop pointer for fifo 
                // if write == read, no elements in buffer 
                // if write > read -> (write - read) elements in buffer
                // if write < read -> ((numElements - read) + write + 1) elements in buffer 
                uint8_t write_index = 0; 
                uint8_t read_index = 0; 


}; 

/* -------------------------------------------------------------------------- */

#endif