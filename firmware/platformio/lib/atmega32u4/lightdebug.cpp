#include "lightdebug.h"
#include <avr/io.h>

using namespace std; 

void configureLEDs()
{
        // colored leds
        DDRF |= ((1 << PF7) | (1 << PF6) | (1 << PF5) | (1 << PF4));
        
        // white leds 
        // BIT ORDER LEFT TO RIGHT
        // PB3, PB2, PB1, PD7, PD6, PD4, PD1, PD0
        DDRD |= ((1 << PD0) | (1 << PD1) | (1 << PD4) | (1 << PD6) | (1 << PD7));
        DDRB |= ((1 << PB1) | (1 << PB2) | (1 << PB3)); 

        return;
}

void print(const uint8_t num)
{
        // print first two LSB by masking, clearing, and setting PORTD
        PORTD &= ~((1 << PD0) | (1 << PD1));
        PORTD |= (num & ((1 << PD0) | (1 << PD1))); 

        // print PD4 PD6, PD7, with third bit in number 
        PORTD &= ~((1 << PD4) | (1 << PD6) | (1 << PD7)); 
        PORTD |= ((num & (1 << 2)) << 2);
        PORTD |= ((num & ((1 << 3) | (1 << 4))) << 3);

        // print PB1, PB2, PB3
        PORTB &= ~((1 << PB1) | (1 << PB2) | (1 << PB3)); 
        PORTB |= ((num & ((1 << 5) | (1 << 6) | (1 << 7))) >> 4); 
        return; 
}

void redOn()
{
        PORTF |= (1 << PF7);
        return; 
}

void redOff()
{
        PORTF &= ~(1 << PF7); 
        return; 
}

void yellowOn()
{
        PORTF |= (1 << PF6); 
        return; 
}

void yellowOff()
{
        PORTF &= ~(1 << PF6);
        return; 
}

void blueOn()
{
        PORTF |= (1 << PF5); 
        return; 
}

void blueOff()
{
        PORTF &= ~(1 << PF5); 
        return; 
}

void greenOn()
{
        PORTF |= (1 << PF4); 
        return;
}

void greenOff()
{
        PORTF &= ~(1 << PF4);
        return; 
}
