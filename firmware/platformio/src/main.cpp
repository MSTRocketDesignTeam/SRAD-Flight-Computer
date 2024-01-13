#include <stdint.h>
#include <time.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <lightdebug.h>
#include <powersaving.h>
#include <cdcserial.h>

using namespace std; 

//const uint8_t toSend[65] = {
//        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
//        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 
//        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 
//        57, 58, 59, 60, 61, 62, 63, 64
//};

int main()
{
        //setup debug leds
        configureLEDs(); 
        DDRC |= (1 << PC7);

        //enable interrupts globally 
        sei();

        uint8_t state = 0; 
        uint32_t last_run_ms = 0; 
        Time.delayMs(5000);

        uint8_t temp = 1 + '0'; 
        while (true)
        {

                Serial.write(temp); 

                // Heartbeat LED 
                if ((Time.millis() - last_run_ms) > 50) {
                        if (state) {
                                PORTC &= ~(1 << PC7); 
                                state = 0; 
                        } else {
                                PORTC |= (1 << PC7); 
                                state = 1;
                        }
                        last_run_ms = Time.millis(); 
                }
        }
        return 0;
}