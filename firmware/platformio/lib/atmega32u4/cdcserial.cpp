#include "cdcserial.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "time.h"

using namespace std; 

/* ------------------------ SERIALCLASS_IMPLMENTATION ----------------------- */
SerialClass::SerialClass()
{
        // Initialize USB interface. (atmega32u4, pg. 266)
        // Initialization process is based off of Arduino CDC Serial Code
        // Additional Information is from here: 
        // https://kampi.gitbook.io/avr/lets-use-usb/initialize-the-usb-as-a-usb-device

        // In this constructor, the following actions will be performed
        // 1. USB regulator will be configured
        // 2. USB controller will be reset to a known state 
        // 3. VBUS interrupts enabled 
        // 4. 

        // initial state 
        state = BUS_DEFAULT_STATE; 



        //! TODO: CHECK USBCORE.cpp FOR SOME FIXES RELATED TO THE CLOCK PLL 

        // Disable to reset the USB controller
        // Then enable to put into known idle state (atmega32u4, pg.267, 270)
        USBCON &= ~(1 << USBE); 
        USBCON |= (1 << USBE); // USB controller now in known idle state

        // Enable the USB clock (atmega32u4, pg. 267)
        USBCON &= ~(1 << FRZCLK);

        // The SRAD board bypasses the internal USB regulator, however the 
        // Arduino micro used for development requires it. Uncomment the following
        // line corresponding to the target device (atmega32u4, pg. 267, 256)
        //UHWCON &= ~(1 << UVREGE); // SRAD Board (disable regulator)
        UHWCON |= (1 << UVREGE); // Arduino Micro (enable regulator)

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

}

uint_fast8_t SerialClass::read() 
{
        
        return 0; 
}

uint_fast8_t SerialClass::write(const uint_fast8_t data)
{
        return data; 
}
/* -------------------------------------------------------------------------- */

/* ----------------------- INTERRUPT_SERVICE_ROUTINES ----------------------- */
// All other USB interrupts (atmega32u4, pg. 262) 
ISR(USB_GEN_vect)
{
        ;
}

// Endpoint Interrupts (atmega32u4, pg. 262)
ISR(USB_COM_vect)
{
        ; 
}
/* -------------------------------------------------------------------------- */

// Create Global Serial Object
//---------------------------
SerialClass Serial; 
//---------------------------
