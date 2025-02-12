#include <Arduino.h>
#include "gpio.h"
#include "board.h"
#include <Adafruit_FRAM_SPI.h>

void setup()
{
        // init various components on the board
        boardInit(); 

        // Set set all (CS) to be HIGH state (disabled)
        gpioInit(PIN::HG_ACCEL_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::IMU_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::BARO_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::TEL_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 
        gpioInit(PIN::FRAM_CS, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S); 

        // The FRAM !(HOLD) is not used by the current library, set HIGH to disable
        gpioInit(PIN::FRAM_HOLD, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);

        // led pins for debugging 
        gpioInit(PIN::LED_R, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);
        gpioInit(PIN::LED_G, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);
        gpioInit(PIN::LED_B, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);

        // wait for serial console opens (for debug)
        while (!Serial) { 
                delay(1);  
        }

        // send init message
        gpioSet(PIN::LED_R, PIN_STATE::LOW_S); 
        delay(1000); 
        Serial.println(F("board init complete")); 
        gpioSet(PIN::LED_R, PIN_STATE::HIGH_S);

        // initalize the FRAM 
        Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(PIN::FRAM_CS); 
        if (fram.begin()) {
                Serial.println(F("fram connected"));
        } else {
                Serial.println(F("fram connection issue"));
                while (true) 
                {
                        delay(1);
                }
        }

        // this chip uses 3 bytes for addressing size pg. 9
        fram.setAddressSize(3); 

        // enable writing to the fram 
        fram.writeEnable(true); //not sure if this needs to be disabled to read? 

        // read all 262,144 8 byte words
        // byte addressable, zero indexed
        gpioSet(PIN::LED_B, PIN_STATE::LOW_S);
        Serial.println(F("reading from FRAM"));
        uint8_t tempval = 0;
        uint8_t readError = 0;  
        for (uint32_t i = 0; i < 262144; i++)
        {
                tempval = fram.read8(i); // read the value stored in the fram
                
                // check if the value is correct 
                if (tempval != (static_cast<uint8_t>(i & 0xFF))) {
                        // error occured
                        Serial.println(F("Read Data Mismatch!"));
                        readError++; 
                        gpioSet(PIN::LED_R, PIN_STATE::LOW_S); //error LED
                }
        }
        gpioSet(PIN::LED_B, PIN_STATE::HIGH_S);

        if (readError == 0) {
                Serial.println(F("Fram Read Verified Successfully"));
                gpioSet(PIN::LED_G, PIN_STATE::LOW_S);
        } else {
                Serial.print(F("There were "));
                Serial.print(readError);
                Serial.print(F(" errors detected!"));
        }

        while (true) { ; } // never end 
        return; 
}

void loop()
{
        ;
}