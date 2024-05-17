#include <Arduino.h>
#include <MS5611_SPI.h> // Barometer Library 
#include <Adafruit_FRAM_SPI.h> // FRAM Library
#include <SparkFun_KX13X.h> // High G Accelerometer Library
#include <LoraSx1262.h> // SX1262 LORA Module Library 

#include "globals.h" // store random global variables
#include "pyro.h"
#include "led.h" // SRAD led abstraction
#include "timer.h" // timer wrapper 
#include "serialComs.h" // PC <---> SRAD communiation

// LED (ACTIVE LOW) --------
// RED: PC6
// GREEN: PB5
// BLUE: PD6

// Pyro Channels -- (ACTIVE HIGH) ----
// CH1 Fire: PB0 
// CH1 Detect: PB4
// CH2 Fire: PD5
// CH2 Detect: PB6
// -----------------------------------

// SPI -----------------
// SCK: PB1 (D15)
// MOSI: PB2 (D16)
// MISO: PB3 (D14)
// Baro. CS: PF5 (D20)
// LORA  CS: PF4 (D21)
// HG Accel Cs: PF6 (D19)

//LoraSx1262 radio; 
//byte* payload = "Hello world.  This a pretty long payload. We can transmit up to 255 bytes at once, which is pretty neat if you ask me";

void setup()
{
        // Initialize the USB interface ------
        Serial.begin(9600); 
        UHWCON &= ~(1 << UVREGE); // SRAD Board (disable regulator)
        DDRC &= ~(1 << PC7); // disable the buzzer pin 
        // -----------------------------------

        // init SPI Bus
        SPI.begin();
        // -----------

        // Intialize Components --------------
        pyro_init(); 

        // disable the hold pin for fram (not used by lib)
        DDRF |= (1 << FRAM_HOLD_P); 
        PORTF |= (1 << FRAM_HOLD_P); // pg 2 datasheet

        // BARO
        ms5611.begin(); // connect to barometer
        ms5611.setOversampling(OSR_STANDARD); // 10x oversampling rate 

        // HG ACCEL
        kxAccel.begin(HG_ACCEL_CS); // initialize accelerometer
        kxAccel.enableAccel(false); // disable so that settings can be changed
        kxAccel.setRange(SFE_KX134_RANGE64G); // 64G range 
        kxAccel.enableDataEngine(); // allows for data ready bit to be set
        kxAccel.setOutputDataRate(4); // pg 26., 12.5hz 
        kxAccel.enableAccel(true); // settings done, enable accelerometer

        // FRAM
        fram.begin();

        // RADIO
        radio.begin(); //start the radio
        radio.configSetFrequency(917500000);
        // -----------------------------------

        led_init(); // SRAD status LED 
        
        delay(2000); 

        // turn on the camera (any pyro channel)
        pyro_ch1(true); 
        pyro_ch2(true); 

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
        /* -------------------------- READ SENSORS -------------------------- */
        if (readSensors) {
                // Time to read all the sensors 
                ms5611.read(); // barometer will take measurement 
                //Serial.print("T:\t");
                //Serial.print(ms5611.getTemperature(), 2);
                //Serial.print("\tP:\t");
                //Serial.println(ms5611.getPressure(), 2);

                outputData myData; 
                kxAccel.getAccelData(&myData);
                // Serial.print(myData.xData, 4);
                // Serial.print(' ');
                // Serial.print(myData.yData,4);
                // Serial.print(' ');
                // Serial.print(myData.zData, 4); 
                // Serial.println(' '); 

                //fram.writeEnable(true);
                //fram.read()
                //fram.write();
        }
        /* ------------------------------------------------------------------ */
        // Timer test(1000); 

        // uint8_t payload[5] = {'h', 'e', 'l', 'l', 'o'};
        // while (true)
        // {
        //         while (test)
        //         {
        //                 led_g(255);
        //                 radio.transmit(payload, 5); 
        //         }
        //         delay(10); 
        //         led_g(0);
        // }

        // while (true)
        // {
        //         while (test)
        //         {
        //                 led_g(255);
        //                 radio.lora_receive_blocking(payload, 5, 100); 
        //                 for (uint8_t i = 0; i < 5; i++)
        //                 {
        //                         Serial.write(payload[i]);
        //                 }
        //                 Serial.println(); 
        //         }
        //         delay(10); 
        //         led_g(0);
        // }


        // delay(10000); 
        // led_r(255);
        // pyro_ch1(true); 
        // delay(30000); 
        // led_r(0);
        // pyro_ch1(false); 

        // led_g(128); 
        // while (true) { ; }

        //communicate(); 
        // led_r(100); 
        // radio.transmit(payload, strlen(payload)); 
        // led_r(0); 

        //delay(1000); 
        // led_r(255);
        // delay(100);
        // led_r(0);
        // delay(100);
        // led_g(255);
        // delay(100);
        // led_g(0);
        // delay(100);
        // led_b(255);
        // delay(100);
        // led_b(0); 
        // delay(100);
        

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