// Author: Seth Sievers
// Date: 4/19/24
// Desc: Implementation file for led abstraction 

// Last Modified: 4/19/24

#include <Arduino.h>
#include "led.h"

/* ----------------------------------- LED ---------------------------------- */
// LED (ACTIVE LOW) --------
// RED: PC6 [not(OC4A)]
// GREEN: PB5 [not(OC4B)]
// BLUE: PD6 [not(OC4D)]

void led_init() {
        // The LEDs are active low, set them high and init the pins 
        PORTC |= (1 << PC6); 
        //PORTC &= ~(1 << PC6);
        PORTB |= (1 << PB5); 
        //PORTB &= ~(1 << PB5); 
        PORTD |= (1 << PD6); 
        DDRC |= (1 << PC6); 
        DDRB |= (1 << PB5);
        DDRD |= (1 << PD6); 

        // NOTE: 10-bit Timer 4 Register Access
        // READ: 1. Read Low Byte 2. Read High Byte
        // WRITE: 1. Write High Byte 2. Write Low Byte

        // Timer 4, use fast pwm mode
        // Enable OC4A (pg. 164)
        TCCR4A &= ~(1 << COM4A1);
        TCCR4A |= (1 << COM4A0); 

        // Enable OC4B 
        TCCR4A &= ~(1 << COM4B1);
        TCCR4A |= (1 << COM4B0); 

        // Enable OC4D (pg. 169)
        TCCR4C &= ~(1 << COM4D1);
        TCCR4C |= (1 << COM4D0); 

        // Enable Fast PWM Mode for outputs (pg. 171)
        TCCR4A |= ((1 << PWM4A) | (1 << PWM4B)); 
        TCCR4C |= (1 << PWM4D);
        TCCR4D &= ~((1 << WGM41) | (1 << WGM40));

        // Set Compare Registers to 0 (0% brightness)
        TC4H = 0; 
        OCR4A = 0; 
        TC4H = 0; 
        OCR4B = 0; 
        TC4H = 0; 
        OCR4D = 0; 

        // Initialize Timer 4 to 0 
        TC4H = 0; 
        TCNT4 = 0; 
        TC4H = ((1 << 0) | (1 << 1)); 
        OCR4C = 0xFF; 

        // Set T4 Prescaler (1x synchronous)
        TCCR4B  &= ~((1 << CS43) | (1 << CS42) | (1 << CS41)); 
        TCCR4B |= (1 << CS40); 

        return; 
}

void led_r(const uint8_t r) {
        TC4H = 0; 
        OCR4A = r; 
        return; 
}

void led_g(const uint8_t g) {
        TC4H = 0; 
        OCR4B = g; 
        return; 
}

void led_b(const uint8_t b) {
        TC4H = 0; 
        OCR4D = b;
        return; 
}

/* -------------------------------------------------------------------------- */