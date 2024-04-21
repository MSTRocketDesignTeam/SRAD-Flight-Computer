#include <Arduino.h>
#include <MS5611_SPI.h> // Barometer Library 
#include <Adafruit_FRAM_SPI.h> // FRAM Library

#include "led.h" // SRAD led abstraction

// LED (ACTIVE LOW) --------
// RED: PC6
// GREEN: PB5
// BLUE: PD6


// SPI -----------------
// SCK: PB1 (D15)
// MOSI: PB2 (D16)
// MISO: PB3 (D14)
// Baro. CS: PF5 (D20)




void setup()
{
        // Initialize the USB interface ------
        Serial.begin(9600); 
        DDRC &= ~(1 << PC7); // disable the buzzer pin 
        // -----------------------------------

        led_init(); // SRAD status LED 
        
}

void loop()
{
        
        led_r(64);
        delay(100);
        led_r(0);
        delay(100);
        led_g(32);
        delay(100);
        led_g(0);
        delay(100);
        led_b(32);
        delay(100);
        led_b(0); 
        delay(100);
        

        /*
        led_g(254); 
        led_b(254);
        led_r(254);
        delay(30); 
        led_g(0); 
        led_b(0);
        led_r(0); 
        delay(30);
        */ 
}