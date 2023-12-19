#include <stdint.h>
#include <time.h>
#include <avr/io.h>

using namespace std; 

int main()
{
        DDRC |= (1 << PC7); 

        while (true)
        {
                PORTC ^= (1 << PC7); 
                Time.delayMs(100); 
        }
        return 0;
}