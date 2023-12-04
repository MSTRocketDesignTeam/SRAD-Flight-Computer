#include "test.h"
#include <avr/io.h> //avr only

// This implementation file should be compiled when the avr environment is chosen

void test_func() 
{
        // turn on the red LED on the SRAD board
        DDRC |= (1 << PC6); 
        PORTC |= (1 << PC6);
        return;
}