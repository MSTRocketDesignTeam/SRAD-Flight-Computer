#include "cdcserial.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

using namespace std; 

/* ------------------------ SERIALCLASS_IMPLMENTATION ----------------------- */
SerialClass::SerialClass()
{
        // Initialization process is based off of Arduino CDC Serial Code
        // Additional Information is from here: 
        // https://kampi.gitbook.io/avr/lets-use-usb/initialize-the-usb-as-a-usb-device

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

        // 16MHz clock therefore set PINDIV for PLL input (atmega32u4, pg. 40)
        PLLCSR |= (1 << PINDIV); 

        // Enable the PLL (atmega32u4, pg. 40-41)
        PLLCSR |= (1 << PLLE);

        // Wait for PLL to lock to accurate clock (atmega32u4, pg. 40-41)
        while (!((1 << PLOCK) & PLLCSR)) {;}



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
ISR(USB_GEN_vect)
{
        ;
}

ISR(USB_COM_vect)
{
        ; 
}
/* -------------------------------------------------------------------------- */

// Create Global Serial Object
//---------------------------
SerialClass Serial; 
//---------------------------
