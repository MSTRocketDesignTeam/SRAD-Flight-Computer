#include <Arduino.h>
#include <MS5611_SPI.h> // Barometer Library 
#include <Adafruit_FRAM_SPI.h> // FRAM Library
#include <SparkFun_KX13X.h> // High G Accelerometer Library
#include <LoraSx1262.h> // SX1262 LORA Module Library 

#include "board.h"
#include "storage.h"
#include "gpio.h"
#include "filtering.h"
#include "accel.h"
#include "baro.h"
#include "LED.h"

/* ---------------------------------- SETUP --------------------------------- */
void setup() 
{
        // Intialize the board and various libraries 
        boardInit(); 
        delay(1); // make sure everything is initialized 

        // enable storage
        storage.init();
        delay(1);

        // initialize the sensors 
        accel.init(); 
        baro.baroInit(); 
        delay(1);

        // init the sum variables 
        filter.init(); 
        delay(1); 

        //! CHANNEL 2 ENABLED AS CAMERA POWER
        delay(1000); 
        gpioSet(PIN::CH2_FIRE, PIN_STATE::HIGH_S); 




        // gpioInit(PIN::LED_R, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // red on
        // while (!Serial) { ; } // wait for pc connection 
        // gpioSet(PIN::LED_R, PIN_STATE::HIGH_S); // red off
        // delay(10000); 

        // // Storage Testing Code
        // storage.init();
        // storage.eraseAll(); //! Need to properly have the code set the storage state on writes/reads, done??
        // gpioInit(PIN::LED_B, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // blue on 
        // Serial.print(F("Before Storage State: ")); 
        // switch (storage.getState()) {
        //         case (Storage::CONTAINS_FLIGHT):
        //                 Serial.println(F("CONTAINS FLIGHT"));
        //                 break; 
        //         case (Storage::EMPTY):
        //                 Serial.println(F("EMPTY"));
        //                 break;
        //         case (Storage::WRITABLE):
        //                 Serial.println(F("WRITABLE"));
        //                 break; 
        //         case (Storage::FULL):
        //                 Serial.println(F("FULL"));
        //                 break;  
        // }
        // while (storage.getState() != Storage::FULL)
        // {
        //         static uint32_t i = 0; 
        //         // write various packets to attempt to read back after
        //         storage.writeAccel(i, 0, (static_cast<uint16_t>(1) << 15)/2, (static_cast<uint16_t>(1) << 15)); 
        //         storage.writeGyro(i, (static_cast<uint16_t>(1) << 15), (static_cast<uint16_t>(1) << 15)/2, 0); 
        //         storage.writePressure(i, 0); 
        //         storage.writePressure(i, (static_cast<uint32_t>(1) << 31));
        //         i++; 
        // }
        // Serial.print(F("After Storage State: ")); 
        // switch (storage.getState()) {
        //         case (Storage::CONTAINS_FLIGHT):
        //                 Serial.println(F("CONTAINS FLIGHT"));
        //                 break; 
        //         case (Storage::EMPTY):
        //                 Serial.println(F("EMPTY"));
        //                 break;
        //         case (Storage::WRITABLE):
        //                 Serial.println(F("WRITABLE"));
        //                 break; 
        //         case (Storage::FULL):
        //                 Serial.println(F("FULL"));
        //                 break;
        // }
        // gpioSet(PIN::LED_B, PIN_STATE::HIGH_S); // blue off
        // delay(5000); 

        // gpioInit(PIN::LED_R, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // red on 
        // // dump the FRAM to the serial terminal 
        // storage.printFRAM(); 
        // gpioSet(PIN::LED_R, PIN_STATE::HIGH_S); // red off
        // delay(5000); 

        // gpioInit(PIN::LED_G, PIN_MODE::OUTPUT_M, PIN_STATE::LOW_S); // green on -> Done 

        // while (true) { ; }
        return; 
}
/* -------------------------------------------------------------------------- */

/* ---------------------------------- LOOP ---------------------------------- */
void loop() 
{
        // SERIAL COMMANDS ------------------------------------------
        // 'S' - Status: Altimeter will print its current state 
        // 'E' - Erase: Altimeter will erase its internal storage 
        // 'R' - Read: Prints stored data to terminal in hex 
        // 'B' - Boost: Set the flight state to boost for debugging
        // Using very simple command structure for handling serial data 
        // Single character command simplifies the command identificiation
        uint8_t r = 0; 
        if (Serial) { // if this is not initialized then print statements will hang
                // when there is no serial data available, read will return -1 = 255
                r = Serial.read(); 

                if (r != 0xFF) { // -1 in binary is 0xFF
                        // Communication has been received 
                        SET_G(1); 

                        // echo so that putty does not require additional echo settings
                        if ((r != '\n') && (r != '\r')) {
                                Serial.print(static_cast<char>(r)); 
                                Serial.print('\n');
                                Serial.print('\r');
                                SET_R(0);
                                SET_B(0); 
                        }

                        // temp var 
                        uint8_t temp = 0;

                        // Switch case to handle commands 
                        switch (r)
                        {
                                case ('S'):
                                        // STATUS -------------
                                        Serial.println(F("--- STATUS ---"));
                                        Serial.print(F("STORAGE_STATE: "));
                                        temp = storage.getState(); 
                                        switch (temp)
                                        {
                                                case (Storage::CONTAINS_FLIGHT):
                                                        Serial.println(F("CONTAINS FLIGHT"));
                                                        break; 
                                                case (Storage::EMPTY):
                                                        Serial.println(F("EMPTY"));
                                                        break;
                                                case (Storage::WRITABLE):
                                                        Serial.println(F("WRITABLE"));
                                                        break; 
                                                case (Storage::FULL):
                                                        Serial.println(F("FULL"));
                                                        break;
                                                default:
                                                        Serial.print(F("INVALID, "));
                                                        Serial.println(temp, HEX);
                                                        break; 
                                        }
                                        Serial.print(F("AccelXAvg: ")); 
                                        Serial.println(filter.getXAccelAvg(), 2);
                                        Serial.print(F("AccelYAvg: ")); 
                                        Serial.println(filter.getYAccelAvg(), 2);
                                        Serial.print(F("AccelZAvg: ")); 
                                        Serial.println(filter.getZAccelAvg(), 2);
                                        Serial.print(F("AccelMag: "));
                                        Serial.println(sqrt(pow(filter.getXAccelAvg(),2) + pow(filter.getYAccelAvg(),2) + pow(filter.getZAccelAvg(),2)));
                                        Serial.print(F("PressureAvg: ")); 
                                        Serial.println(filter.getPressureAvg(), 2);

                                        temp = static_cast<uint8_t>(filter.getState());
                                        switch (static_cast<Filter::ROCKET_STATE>(temp))
                                        {
                                                case (Filter::ROCKET_STATE::LAUNCH_WAIT):
                                                        Serial.println(F("LAUNCH WAIT"));
                                                        break;
                                                case (Filter::ROCKET_STATE::BOOST):
                                                        Serial.println(F("BOOSTING"));
                                                        break;
                                                case (Filter::ROCKET_STATE::APOGEE):
                                                        Serial.println(F("APOGEE"));
                                                        break;
                                                case (Filter::ROCKET_STATE::FALL):
                                                        Serial.println(F("FALL"));
                                                        break;
                                                case (Filter::ROCKET_STATE::LANDED):
                                                        Serial.println(F("LANDED"));
                                                        break;
                                                default:
                                                        Serial.print(F("INVALID, "));
                                                        Serial.println(temp, HEX);
                                                        break;
                                        }
                                        Serial.print(F("CH1 Armed?: "));
                                        Serial.println(gpioRead(PIN::CH1_DETECT));
                                        Serial.print(F("CH2 Armed?: "));
                                        Serial.println(gpioRead(PIN::CH2_DETECT));

                                        break; 
                                case ('E'):
                                        // ERASE --------------
                                        Serial.print(F("Erasing Storage..."));
                                        storage.eraseAll();
                                        Serial.println(F("Done."));
                                        filter.flightState = Filter::ROCKET_STATE::LAUNCH_WAIT;
                                        break;
                                case ('R'):
                                        // READ ---------------
                                        Serial.println(F("Reading Storage..."));
                                        Serial.println(F("--------------------"));
                                        storage.printFRAM(); 
                                        Serial.println(F("--------------------"));
                                        Serial.println(F("Done."));
                                        break;
                                case ('T'):
                                        // TEST --------------
                                        Serial.print(F("Testing Pyro Channel..."));
                                        SET_R(1);
                                        gpioSet(PIN::CH1_FIRE, PIN_STATE::HIGH_S);
                                        delay(1000);
                                        gpioSet(PIN::CH1_FIRE, PIN_STATE::LOW_S);
                                        SET_R(0); 
                                        Serial.println(F("Done."));
                                        break;
                                case ('B'):
                                        // BOOST ------------
                                        filter.flightState = Filter::ROCKET_STATE::BOOST;
                                default:
                                        break; 
                        }
                        Serial.print('\n'); // newline spacer 
                        Serial.print('\r');
                        SET_G(0); 
                }
                
        }
        
        // ----------------------------------------------------------

        // display current state as LED 
        switch(filter.getState())
        {
                case (Filter::ROCKET_STATE::LAUNCH_WAIT):
                        SET_B(1);
                        SET_G(0);  
                        SET_R(0); 
                        break;
                case (Filter::ROCKET_STATE::BOOST):
                        SET_B(0); 
                        SET_R(0); 
                        SET_G(1); 
                        break; 
                case (Filter::ROCKET_STATE::APOGEE): 
                        SET_B(0); 
                        SET_R(1); 
                        SET_G(0); 
                        break; 
                case (Filter::ROCKET_STATE::FALL):
                        SET_B(0); 
                        SET_R(1); 
                        SET_G(0); 
                        break; 
                case (Filter::ROCKET_STATE::LANDED):
                        SET_R(1); 
                        SET_B(1); 
                        SET_B(1); 
                        break; 
                default: 
                        break; 
        }

        // read from sensors and store in buffer
        filter.sample(); 

        return; 
}
/* -------------------------------------------------------------------------- */
