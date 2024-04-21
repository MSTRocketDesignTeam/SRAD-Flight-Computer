// Author: Seth Sievers
// Date: 4/1/24
// Desc: implementation file for the PC <---> SRAD communiation functionality 

#include "serialComs.h"
#include "globals.h"
#include "timer.h"
#include <Arduino.h>

#include "led.h"

// Global ------
Timer doComCheck(250); 

// -------------


/* ------------------------------- serialComs ------------------------------- */
void communicate() {
        static uint8_t comState = 0; //state of the current communication 
        // 0: no connection 
        // 1: handshake [ PC -> 0xAA, SRAD -> 0xBB]
        // 2: command mode 

        if (doComCheck) { // limit the check interval 
                // if at any time the usb is reset or if vbus is low 
                if (!Serial || !(USBSTA & (1 << VBUS))) { 
                        comState = 0;
                        led_g(0);
                        led_b(0);
                }

                led_r(255);  

                switch (comState) {
                        case 0: // NO CONNECTION
                                led_g(0); 
                                // if usb is connected, move to next state 
                                if (Serial) {
                                        comState = 1; 
                                }
                                break; 
                        case 1: // HANDSHAKE 
                                led_g(2); 
                                if (Serial.available() > 0) {
                                        if (Serial.read() == 0xAA) {
                                                // PC handshake successful
                                                // send SRAD handshake 
                                                Serial.write(0xBB); 
                                                Serial.flush(); 
                                                comState = 2; 
                                        }
                                }
                                break; 
                        case 2: // COMMAND 
                                led_b(100); 
                                break; 
                }

                delay(50); 
                led_r(0);
        }

}
/* -------------------------------------------------------------------------- */
