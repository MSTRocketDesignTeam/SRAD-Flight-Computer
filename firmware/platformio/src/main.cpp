#include <stdint.h>
#include <time.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <lightdebug.h>
#include <powersaving.h>
#include <cdcserial.h>

using namespace std; 

uint8_t rx_i = 0;
uint8_t key[7] = {"hello?"};
uint8_t hllWrld[13] = {"hello world!"};
uint8_t err[25] = {"incorrect key, try again"};

int main()
{
        //setup debug leds
        configureLEDs(); 
        DDRC |= (1 << PC7);

        //enable interrupts globally 
        sei();
        //uint8_t test = 0;  
        uint8_t state = 0; 
        Time.delayMs(5000);
        while (true)
        {

                // USB clock frozen? 
                if (USBCON & (1 << FRZCLK)) {
                        //blueOn();
                } else {
                        //blueOff(); 
                }


                // heartbeat led
                Time.delayMs(50); 
                //test++; 
                //print(0x9F); 
                if (state) {
                        PORTC &= ~(1 << PC7); 
                        state = 0; 
                } else {
                        PORTC |= (1 << PC7); 
                        state = 1;
                }


        }
        return 0;
}