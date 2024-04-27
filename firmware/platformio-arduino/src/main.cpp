#include <Arduino.h>
#include <MS5611_SPI.h> // Barometer Library 
#include <Adafruit_FRAM_SPI.h> // FRAM Library
#include <SparkFun_KX13X.h> // High G Accelerometer Library
#include <LoraSx1262.h> // SX1262 LORA Module Library 

#include "globals.h" // store random global variables
#include "led.h" // SRAD led abstraction
#include "timer.h" // timer wrapper 
#include "serialComs.h" // PC <---> SRAD communiation

// LED (ACTIVE LOW) --------
// RED: PC6
// GREEN: PB5
// BLUE: PD6


// SPI -----------------
// SCK: PB1 (D15)
// MOSI: PB2 (D16)
// MISO: PB3 (D14)
// Baro. CS: PF5 (D20)
// LORA  CS: PF4 (D21)


//LoraSx1262 radio; 
//byte* payload = "Hello world.  This a pretty long payload. We can transmit up to 255 bytes at once, which is pretty neat if you ask me";

void setup()
{
        // Initialize the USB interface ------
        Serial.begin(9600); 
        UHWCON &= ~(1 << UVREGE); // SRAD Board (disable regulator)
        DDRC &= ~(1 << PC7); // disable the buzzer pin 
        // -----------------------------------

        led_init(); // SRAD status LED 
        
        delay(10); 

        // if (radio.begin()) {
        //         led_g(50); 
        // } else {
        //         led_r(50); 
        // }
        // delay(10); 
        // led_g(0); 
}

void loop()
{




        communicate(); 
        // led_r(100); 
        // radio.transmit(payload, strlen(payload)); 
        // led_r(0); 

        //delay(1000); 
        //led_r(255);
        //delay(100);
        //led_r(0);
        //delay(100);
        //led_g(255);
        //delay(100);
        //led_g(0);
        //delay(100);
        //led_b(255);
        //delay(100);
        //led_b(0); 
        //delay(100);
        

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