#include "lightdebug.h"
#include <avr/io.h>

using namespace std; 

void configureLEDs()
{
        DDRF |= ((1 << PF7) | (1 << PF6) | (1 << PF5) | (1 << PF4));
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
