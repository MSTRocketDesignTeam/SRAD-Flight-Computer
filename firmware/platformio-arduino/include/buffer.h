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
                Buf()
                {
                        for (uint8_t i = 0; i < numElements; i++)
                        {
                                bufArr[i] = 0; 
                        }
                }

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
                        if (getFreeElements() == 0) {
                                return NULL; 
                        }

                        // writing to queue, advance write_index
                        uint8_t old_index = write_index;
                        write_index = (write_index + 1) % (numElements); 

                        // return ptr to write element 
                        return (&bufArr[old_index]);
                }

                const T * const dequeue()
                {
                        // is there anything to dequeue? 
                        if (getNumElements() == 0) {
                                return NULL; 
                        }

                        // reading from queue, advance read_index
                        uint8_t old_index = read_index; 
                        read_index = (read_index + 1) % (numElements); 

                        // return ptr to read element 
                        return (&bufArr[old_index]); 
                }
                
                // returns number of elements in queue 
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

                // returns amount of free space in queue 
                uint8_t getFreeElements()
                {
                        uint8_t temp = numElements - getNumElements(); 
                        if (temp == 1) {
                                return 0; // lose one spot to avoid ambiguity
                        }
                        return temp; 
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