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
        sensorStorage.cfgInit(); // load persistent values from fram to ram 

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
        if (readSensors && sensorStorage.inFlight()) {
                led_g(255); 
                // Time to read all the sensors 
                dataPkt tempData; //store the sensor data until transferred to buffer

                // Barometer
                ms5611.read(); // barometer will take measurement 
                // For ease of storage, going to store pressure as an integer
                // assuming 1100mBar as atmospheric maximum, let 
                // counts / mBar = 65535/1150 = 13107/230
                // scale the float and then cast to an integer
                tempData.pressure = static_cast<uint16_t>(ms5611.getPressure() * (13107.0 / 230.0)); 

                // Serial.print("T:\t");
                // Serial.print(ms5611.getTemperature(), 2);
                // Serial.print("\tP:\t");
                // Serial.println(ms5611.getPressure(), 2);

                // Accelerometer
                // Inefficient, should reverse engineer the driver and store bit values directly
                // at 64g range, resolution is 512 LSB per G, 
                // storing as signed int with max value 32767
                // counts / g = 32767 / 64
                outputData myData; 
                kxAccel.getAccelData(&myData);
                tempData.xAccel = static_cast<int16_t>(myData.xData * (32767.0 / 64.0));
                tempData.yAccel = static_cast<int16_t>(myData.yData * (32767.0 / 64.0));
                tempData.zAccel = static_cast<int16_t>(myData.zData * (32767.0 / 64.0));
                // Serial.print(myData.xData, 4);
                // Serial.print(' ');
                // Serial.print(myData.yData,4);
                // Serial.print(' ');
                // Serial.print(myData.zData, 4); 
                // Serial.println(' '); 

                //time
                tempData.time = (millis()/10);

                //store the data in the buffer
                sensorStorage.bufEnqueue(tempData); 

                //fram.writeEnable(true);
                //fram.read()
                //fram.write();
                led_g(0); 
        }

        if (!sensorStorage.inFlight()) {
                led_r(255); // HAS LANDED
        } else {
                led_r(0); 
        }

        // communicate over serial comms
        if (Serial) { //leonardo has serial connected
                led_b(255); 
                if (sensorStorage.inFlight()) {
                        // on the ground still 
                        if (comMessage) {
                                Serial.println(F("FC is saving data, waiting to land."));
                                Serial.println(F(" ")); 
                        }
                } else {
                        // send out the data 
                        switch (comState) 
                        {
                                case 0: // begin 
                                        if (comMessage) {
                                                Serial.println(F("Beginning Data Dump")); 
                                                Serial.println(F("NOTE: THIS DATA WILL NEED PROCESSING"));
                                                Serial.println(F("XAccel, yAccel, zAccel, pressure, time"));
                                                comState = 1;
                                                delay(1000); 
                                        }
                                        break; 
                                case 1: // send out data
                                        for (uint32_t i = 0; i < ((SRAD_FRAM_WORD_LENGTH - 9) / sizeof(dataPkt)); i++)
                                        {
                                                dataPkt tempPkt = sensorStorage.framRead(i); //will read every possible spot, may not be valid
                                                Serial.print(tempPkt.xAccel);
                                                Serial.print(F(","));
                                                Serial.print(tempPkt.yAccel);
                                                Serial.print(F(","));
                                                Serial.print(tempPkt.zAccel);
                                                Serial.print(F(","));
                                                Serial.print(tempPkt.pressure);
                                                Serial.print(F(","));
                                                Serial.println(tempPkt.time);
                                        }
                                        Serial.println(F("DONE!"));
                                        comState = 2; 
                                        break; 
                                case 2: // wait to restart
                                        led_b(0); 
                                        delay(10000); 
                                        comState = 0; 
                                        break; 
                        }
                }
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