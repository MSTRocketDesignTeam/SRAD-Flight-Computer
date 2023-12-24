#include "cdcserial.h"
#include <stdint.h>

using namespace std; 

/* ------------------------ SERIALCLASS_IMPLMENTATION ----------------------- */
SerialClass::SerialClass()
{
        ;
}

uint_fast8_t SerialClass::read() 
{
        return 0; 
}

uint_fast8_t SerialClass::write(const uint_fast8_t data)
{
        return data; 
}
/* -------------------------------------------------------------------------- */

// Create Global Serial Object
//---------------------------
SerialClass Serial; 
//---------------------------
