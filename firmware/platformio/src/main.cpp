#include <stdint.h>
#include <time.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <powersaving.h>
#include <cdcserial.h>

using namespace std; 

int main()
{
        DDRC |= (1 << PC7); 
        sei(); 

        while (true)
        {
                PORTC ^= (1 << PC7); 
                Time.delayMs(500); 
        }
        return 0;
}