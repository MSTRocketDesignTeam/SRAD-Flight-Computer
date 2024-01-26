#include <stdint.h>
#include <time.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
//#include <lightdebug.h>
//#include <powersaving.h>
//#include <cdcserial.h>

using namespace std; 

int main()
{
        //enable interrupts globally 
        sei();

        //setup LED pins for testing
        PORTD = (1 << PD6);
        DDRD = ~(1 << PD6);
        PORTB = (1 << PB5); 
        PORTC = (1 << PC6); 

        while (true)
        {
                ;
        }
        return 0;
}