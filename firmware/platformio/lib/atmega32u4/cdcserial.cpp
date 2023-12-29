#include "cdcserial.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "time.h"
#include <util/atomic.h>
#include "lightdebug.h"
#include "powersaving.h"
#include "cdcserialdefines.h"

using namespace std; 

/* ------------------------ SERIALCLASS_IMPLMENTATION ----------------------- */
SerialClass::SerialClass()
{
        // TODO: REMOVE DEBUG LIGHT
        redOff(); 
        greenOff();
        blueOff(); 
        yellowOff(); 

        // Initialize the USB system 
        initUSB(); 



/*
        //! TODO: CHECK USBCORE.cpp FOR SOME FIXES RELATED TO THE CLOCK PLL 



        // Enable the USB clock (atmega32u4, pg. 267)
        USBCON &= ~(1 << FRZCLK);



        // Connect the VBUS pad to the USB controller (atmega32u4, pg. 267)
        USBCON |= (1 << OTGPADE); 

        // Enable Full Speed Device. (atmega32u4, pg. 281)
        UDCON &= ~(1 << LSM);

        //! TODO: IS VBUSTE required? not set currently 

        // Attach to the USB bus. Connects pullup to D+ and causes host to reset
        // The USB controller in Atmega32u4 (atmega32u4, pg. 281)
        UDCON &= ~(1 << DETACH); 



        // Enable the PLL (atmega32u4, pg. 40-41)
        PLLCSR |= (1 << PLLE);

        // Wait for PLL to lock to accurate clock (atmega32u4, pg. 40-41)
        while (!((1 << PLOCK) & PLLCSR)) {;}
*/
}

inline void SerialClass::initUSB()
{
        // Initialize USB interface. (atmega32u4, pg. 266)
        // Initialization process is based off of Arduino CDC Serial Code
        // Additional Information is from here: 
        // https://kampi.gitbook.io/avr/lets-use-usb/initialize-the-usb-as-a-usb-device

        // In this function, the following actions will be performed
        // 1. Configure the PLL (configure not enable)
        // 2. Reset the USB controller to idle state
        // 3. Configure the USB regulator 
        // 4. Enable VBUS interrupts

        // initial state 
        state = BUS_DEFAULT_STATE; 

        // configure the PLL so when it is enabled it provides correct clock 
        configurePLL(); 

        // Clear to reset the USB controller
        // Then enable to put into known idle state (atmega32u4, pg.267, 270)
        USBCON &= ~(1 << USBE); 
        USBCON |= (1 << USBE); // USB controller now in known idle state

        // Select Full Speed Mode (atmega32u4, pg. 281)
        UDCON &= ~(1 << LSM); 

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) // do not trigger interrupt yet
        {
                // Enable the USB Pad Regulator (for D+ / D-)
                // The SRAD board bypasses the internal USB regulator, however the 
                // Arduino micro used for development requires it. Uncomment the following
                // line corresponding to the target device (atmega32u4, pg. 267, 256)
                //UHWCON &= ~(1 << UVREGE); // SRAD Board (disable regulator)
                UHWCON |= (1 << UVREGE); // Arduino Micro (enable regulator)

                // Enable VBUS change Interrupts (atmega32u4, pg. 267)
                // Enable the USB controller VBUS Pad (atmega32u4, pg. 267)
                USBCON |= ((1 << VBUSTE) | (1 << OTGPADE)) ;
                
                // If VBUS is already high, enable the PLL and unfreeze the clock 
                if (USBSTA & )
        }
        
        // Once the VBUS is applied the PLL should be activated
        return; 
}

inline void SerialClass::configurePLL()
{
        // (atmega32u4, pg. 36)
        // Fuse Configuration will set the CKSEL bits with defaults 

        // 16MHz clock therefore set PINDIV for PLL input (atmega32u4, pg. 40)
        PLLCSR |= (1 << PINDIV); 

        // Select crystal as PLL source (atmega32u4, pg. 41)
        PLLFRQ &= ~(1 << PINMUX); 

        // Configure PLL to output 48MHz (atmega32u4, pg. 41)
        PLLFRQ &= ~((1 << PDIV3) | (1 << PDIV1) | (1 << PDIV0)); 
        PLLFRQ |= (1 << PDIV2); 
        return; 
}

inline void SerialClass::enableUSBCLK()
{
        // To enable clock lock the PLL so it is not disabled by powersaving
        // Using id position of 0
        Pwr.lockPLL(USB_PLL_LOCK_ID); 

        // Now that PLL is enabled, unfreeze the USB clock (atmega32u4, pg. 267)
        USBCON &= ~(1 << FRZCLK);
        return; 
}

inline void SerialClass::disableUSBCLK()
{
        // freeze the USB clock (atmega32u4, pg.267)
        USBCON |= (1 << FRZCLK); 

        // unlock the PLL so that powersaving may disable it
        Pwr.unlockPLL(USB_PLL_LOCK_ID);
        return; 
}

uint_fast8_t SerialClass::read() 
{
        
        return 0; 
}

uint_fast8_t SerialClass::write(const uint_fast8_t data)
{
        return data; 
}

inline void SerialClass::ISR_general() volatile
{
        if (USBINT & (1 << VBUSTI)) {
                USBINT &= ~(1 << VBUSTI); 
                greenOn(); 
        }
        return; 
}

inline void SerialClass::ISR_common() volatile
{
        return; 
}
/* -------------------------------------------------------------------------- */

/* ----------------------- INTERRUPT_SERVICE_ROUTINES ----------------------- */
// All other USB interrupts (atmega32u4, pg. 262) 
ISR(USB_GEN_vect)
{
        Serial.ISR_general(); 
}

// Endpoint Interrupts (atmega32u4, pg. 262)
ISR(USB_COM_vect)
{
        Serial.ISR_common(); 
}
/* -------------------------------------------------------------------------- */

// Create Global Serial Object
//---------------------------
SerialClass Serial; 
//---------------------------
