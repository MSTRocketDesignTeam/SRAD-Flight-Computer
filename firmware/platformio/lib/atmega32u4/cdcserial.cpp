#include "cdcserial.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "time.h"
#include <util/atomic.h>
#include "lightdebug.h"
#include "powersaving.h"
#include "cdcserialdefines.h"
#include <avr/pgmspace.h> //PROGMEM

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

void SerialClass::initUSB()
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
        // 5. Update state variable 
        // 5A. If VBUS is high, configure endpoints and attach device
        // 5B. If VBUS is low, VBUS interrupt will handle connection 
        // 6A. Clear VBUS int to guarantee that it will not be called again  

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
                USBCON |= ((1 << VBUSTE) | (1 << OTGPADE));

                // Enable Required Interrupts 
                // EORSTE: When host resets USB device duiring initialization set the EORSTI flag
                UDIEN |= ((1 << EORSTE)); 
                
                // If VBUS is already high, enable the PLL and unfreeze the clock 
                // If VBUS is not currently high, then the clock will enable when 
                // the VBUSI interrupt is triggered //TODO: IMPLEMENT THIS 
                if (USBSTA & (1 << VBUS)) {
                        // If initially powered, the go ahead and try to connect
                        // Otherwise the connection will be handled by VBUS interrupt 
                        enableUSBCLK(); 

                        // Attach the device so that the host starts reset (atmega32u4, pg. 281)
                        UDCON &= ~(1 << DETACH);

                        // Update the usb state variable 
                        state = BUS_ATTACHED_STATE; 

                        // Clear the VBUSTI flag so that interrupt is not called again (atmega32u4, pg. 268)
                        USBINT &= ~(1 << VBUSTI); 

                } else {
                        state = BUS_UNPOWERED_STATE;
                }
        }
        
        // Once the VBUS is applied the PLL should be activated
        return; 
}

void SerialClass::configurePLL()
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

inline void SerialClass::initEP(const uint_fast8_t epNum, 
        const uint_fast8_t epCFG0, const uint_fast8_t epCFG1)
{
        // Following Ep Setup Procedure (atmega32u4, pg. 271)

        // Select the correct endpoint (atmega32u4, pg. 285)
        UENUM = epNum; 

        // Activate the endpoint (atmega32u4, pg. 286)
        UECONX |= (1 << EPEN); 

        // Configure Direction and Type by setting UECFG0x (atmega32u4, pg. 286)
        UECFG0X = epCFG0; 

        // Configure Endpoint Size and Allocate memory (atmega32u4, pg. 287)
        UECFG1X = (epCFG1);

        // Check to verify activation (atmega32u4, pg. 287)
        if (!(UESTA0X & (1 << CFGOK))) {
                // Invalid State, mark the bus to be reset 
                state = BUS_INVALID_STATE; 
                greenOn();
        }
        
        return; 
}

inline void SerialClass::ISR_general()
{
        // Handle a VBUS transition interrupt
        if (USBINT) { // Other bits always read as 0 
                // VBUS transition occured
                // Clear the Interrupt Flag (atmega32u4, pg. 268)
                USBINT = 0; 
                if (USBSTA & (1 << VBUS)) {
                        // VBUS is high 
                        // Enable the USB Clock 
                        enableUSBCLK(); 

                        // Attach the device so that the host starts reset (atmega32u4, pg. 281)
                        UDCON &= ~(1 << DETACH);

                        // Update the usb state variable 
                        state = BUS_ATTACHED_STATE; 
                } else {
                        // VBUS is low 
                        // TODO: Proper Powersaving procedure

                        // Set interface to be detached
                        UDCON |= (1 << DETACH);

                        // Disable the Clock and unlock PLL
                        disableUSBCLK(); 

                        //Update the state variable 
                        state = BUS_UNPOWERED_STATE; 
                }
        }

        // Handle a USB End of Reset Interrupt 
        // Device has been reset by host 
        if (UDINT & (1 << EORSTI)){
                blueOn(); 
                // EOR Int occured
                // Clear the Flag 
                UDINT &= ~(1 << EORSTI); 

                // state machine to handle invalid transitions 
                switch (state)
                {
                        case (BUS_ATTACHED_STATE): 
                                // Valid condition 
                                state = BUS_EOR_STATE; 

                                // Configure Endpoint 0 
                                initEP(0, EP_CTL_CFG0, EP_CTL_CFG1); 


                                break; 
                        default: 
                                // Invalid - should not occur
                                state = BUS_INVALID_STATE; 
                                break;
                }
        }
        /*
        // utlize state machine to only check required if statements
        // TODO: MAKE SURE THIS WON'T GET CAUGHT IN INFINITE INTERRUPT LOOP
        switch (state)
        {
                case (BUS_UNPOWERED_STATE):
                        // VBUS interrupt 
                        if ()
                        if (USBSTA & (1 << VBUS)) {
                                ; 
                        }
                        break;

                case (BUS_POWERED_STATE):

                        break; 

                case (BUS_INVALID_STATE):
                default: 
                        //TODO: INVALID, reset USB controller 
                        break;
        }
        if (USBINT & (1 << VBUSTI)) {
                USBINT &= ~(1 << VBUSTI); 
                greenOn(); 
        }

        */
        return; 
}

inline void SerialClass::ISR_common() volatile
{
        return; 
}

const SerialClass::USB_DeviceDescriptor_t SerialClass::DeviceDescriptor PROGMEM = 
{
        // https://www.beyondlogic.org/usbnutshell/usb5.shtml#DeviceDescriptors
        sizeof(USB_DeviceDescriptor_t), // .bLength: 18 byte struct 
        0x01, // .bDescriptorType: Specifies device descriptor
        0x0200, // .bcdUSB: USB version, 0x0200 -> USB 2.0 
        0xEF, // .bDeviceClass: Misc Device, https://www.usb.org/defined-class-codes
        0x02, // .bDeviceSubClass: see below and above
        0x01, // .bDeviceProtocol: Interface Asssociation Descriptor
        64, // .bMaxPacketSize0: EP 0 (CTL) has 64 byte buffer
        0x2341, // .idVendor: "Arduino" https://the-sz.com/products/usbid/index.php?v=0x2341&p=&n=
        0x8037, // .idProduct: Default "Micro" ID, might change https://gist.github.com/nebhead/c92da8f1a8b476f7c36c032a0ac2592a
        0x100, // .bcdDevice: Device version assigned by Arduino team 
        1, // .iManufacturer: Manufacturer String Index Offset
        2, // .iProduct: Product String Index Offset 
        3, // .iSerialNumber: Serial Number String Index Offset
        1 // .bNumConfigurations: Only one configuration 
};

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
