#include <stdint.h>
#include <time.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <lightdebug.h>
#include <powersaving.h>
#include <cdcserial.h>

using namespace std; 

int main()
{
        //setup debug leds
        configureLEDs(); 
        DDRC |= (1 << PC7);

        //enable interrupts globally 
        sei(); 
        uint8_t state = 0; 
        while (true)
        {
                if (USBSTA & (1 << VBUS)) {
                        yellowOn();
                } else {
                        yellowOff(); 
                }

                //USBINT |= (1 << VBUSTI); 

                if (USBINT & (1 << VBUSTI)) {
                        blueOn(); 
                } else {
                        blueOff(); 
                }


                Time.delayMs(50); 
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