/*
        This is the implementation file for any functions from the board.h file
*/

#include "board.h"
#include <avr/io.h>

void board_init()
{
        // because of how the SRAD-FC has its USB port connected, we must disable the
        // internal USB voltage regulator
        UHWCON &= ~(1 << UVREGE); // SRAD Board (disable regulator)
        return; 
}