#include "LED.h"
#include "gpio.h"

void LED_R(const uint8_t state)
{
        if (state == 1) {
                gpioSet(PIN::LED_R, PIN_STATE::LOW_S); // Red on 
        } else {
                gpioSet(PIN::LED_R, PIN_STATE::HIGH_S); // Red off 
        }
        return; 
}
void LED_G(const uint8_t state)
{
        if (state == 1) {
                gpioSet(PIN::LED_G, PIN_STATE::LOW_S); // Green on 
        } else {
                gpioSet(PIN::LED_G, PIN_STATE::HIGH_S); // Green off 
        }
        return; 
}
void LED_B(const uint8_t state)
{
        if (state == 1) {
                gpioSet(PIN::LED_B, PIN_STATE::LOW_S); // Blue on 
        } else {
                gpioSet(PIN::LED_B, PIN_STATE::HIGH_S); // Blue off 
        }
        return; 
}