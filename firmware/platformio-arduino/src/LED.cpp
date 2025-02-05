#include "LED.h"
#include "gpio.h"

void SET_R(const uint8_t state)
{
        if (state == 1) {
                gpioSet(PIN::LED_R, PIN_STATE::LOW_S); 
        } else {
                gpioSet(PIN::LED_R, PIN_STATE::HIGH_S);  
        }
        return; 
}
void SET_G(const uint8_t state)
{
        if (state == 1) {
                gpioSet(PIN::LED_G, PIN_STATE::LOW_S); 
        } else {
                gpioSet(PIN::LED_G, PIN_STATE::HIGH_S);  
        }
        return; 
}
void SET_B(const uint8_t state)
{
        if (state == 1) {
                gpioSet(PIN::LED_B, PIN_STATE::LOW_S); 
        } else {
                gpioSet(PIN::LED_B, PIN_STATE::HIGH_S); 
        }
        return; 
}