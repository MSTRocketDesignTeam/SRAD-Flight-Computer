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

                if (Serial.available()) {
                        uint8_t val = Serial.read(); 
                        if (val == key[rx_i]) {
                                rx_i++; 
                        } else {
                                if (val != '\n') {
                                        for (uint8_t i = 0; i < 24; i++)
                                        {
                                                Serial.write(err[i]);
                                        }
                                        Serial.write('\n');
                                        rx_i = 0; 
                                }
                        }

                        if (rx_i == 6) {
                                for (uint8_t i = 0; i < 12; i++) {
                                        Serial.write(hllWrld[i]); 
                                }
                                Serial.write('\n');
                                for (uint8_t j = 0; j < 50; j++)
                                {
                                        for (uint8_t k = 0; k < 50; k++)
                                        {
                                                Serial.write('#'); 
                                        }
                                        Serial.write(' ');
                                        Serial.write((j/10) + '0');
                                        Serial.write('\n');
                                }
                                rx_i = 0; 
                        } 
                }

                /*
                if ((Time.millis() - last_time) > 500) {
                        if (usb_clock_state) {
                                Serial.write('a'); 
                                usb_clock_state = 0; 
                        } else {
                                usb_clock_state = 1; 
                        }
                        last_time = Time.millis(); 
                }
                */ 

        }
        return 0;
}