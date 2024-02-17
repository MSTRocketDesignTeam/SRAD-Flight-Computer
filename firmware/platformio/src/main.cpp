#include <stdint.h>
#include <time.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <lightdebug.h>
//#include <powersaving.h>
#include <cdcserial.h>

using namespace std; 

int main()
{
        //enable interrupts globally 
        sei();

        //setup LED pins for testing
        PORTD |= (1 << PD6); //B
        DDRD |= (1 << PD6);
        PORTB |= (1 << PB5); //G
        DDRB |= (1 << PB5);
        PORTC |= (1 << PC6); //R
        DDRC |= (1 << PC6);
        //print(11);

        // buzzer pin is output 
        PORTC &= ~(1 << PC7);
        DDRC |= (1 << PC7); 

        // pyro channel 1 
        PORTB &= ~(1 << PB0); 
        DDRB |= (1 << PB0); 

        uint32_t last_run_ms = Time.millis(); 
        uint8_t state = 0; 

        Time.delayMs(100);
        while (true)
        {
                //half_period++; 
                /*
                DDRC |= (1 << PC7);
                PORTC |= (1 << PC7);
                _delay_us(125);
                //Time.delayMs(1);
                DDRC &= ~(1 << PC7);
                _delay_us(5);
                PORTC &= ~(1 << PC7); 
                _delay_us(120);
                //Time.delayMs(1); 
                */
                
                PORTD &= ~(1 << PD6); //B
                Time.delayMs(100); 
                PORTD |= (1 << PD6);
                Time.delayMs(100);
                PORTC &= ~(1 << PC6); //R
                Time.delayMs(100);
                PORTC |= (1 << PC6);
                Time.delayMs(100);
                PORTB &= ~(1 << PB5); //G
                Time.delayMs(100);
                PORTB |= (1 << PB5);
                Time.delayMs(100);

                if ((Time.millis() - last_run_ms) > 2000) {
                        state = !state; 
                        if (state) {
                                PORTB |= (1 << PB0); 
                        } else {
                                PORTB &= ~(1 << PB0); 
                        }
                        last_run_ms = Time.millis(); 
                }
                
        }
        return 0;
}