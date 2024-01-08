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
        //uint8_t test = 0;  
        uint8_t state = 0; 
        uint8_t usb_clock_state = 0; 
        uint32_t last_time = 0; 
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
                if ((Time.millis() - last_time) > 2000) {
                        if (usb_clock_state) {
                                Serial.wri 
                                usb_clock_state = 0; 
                        } else {
                                usb_clock_state = 1; 
                        }
                        last_time = Time.millis(); 
                }

        }
        return 0;
}