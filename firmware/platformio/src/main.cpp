//#include <avr/io.h>
#include <stdint.h>

int main()
{
        int8_t signed_integer = -25; 
        uint8_t unsigned_integer = 0;
        unsigned_integer = signed_integer;
        
        while (1) {;}
        return 0;
}