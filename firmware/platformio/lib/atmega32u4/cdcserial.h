/*
This is the hardware implementation of the 'serial' interface used to communicate
with a connected PC. This implemenetation for the Atmega32u4 uses the native USB port
to create a virtual com port. This may very by processor. 
*/

#ifndef CDCSERIAL_h
#define CDCSERIAL_h

#include "serialinterface.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/pgmspace.h> //PROGMEM

using namespace std; 

/* ------------------------------ SERIAL_CLASS ------------------------------ */
class SerialClass : public SerialInterface
{
        public:
                // Constructor
                SerialClass(); 

                // Overrided functions from interface
                uint_fast8_t read() override; 
                uint_fast8_t write(const uint_fast8_t data) override;

                inline void ISR_general(); 

                inline void ISR_common() volatile;

        protected: 
                // Desc: Reset the USB interface 
                // Args: None
                // Returns: Nothing 
                void initUSB(); 

                // Desc: Configures the PLL for generating USB clock 
                // Args: None
                // Returns: None 
                void configurePLL(); 

                // Desc: Unfreezes the USB CLK
                // Args: None
                // Returns: None 
                inline void enableUSBCLK();

                // Desc: Freezes the USB CLK for power savings 
                // Args: None 
                // Returns: None 
                inline void disableUSBCLK(); 

                // Desc: Setup the endpoint 
                inline void initEP(const uint_fast8_t epNum, 
                        const uint_fast8_t epCFG0, const uint_fast8_t epCFG1); 

                volatile uint8_t currentStatus = ((1 << 7) | (0 << 6) | (1 << 5)); // initial status as defined in configuration descriptor
                volatile uint_fast8_t state = BUS_INITIAL_STATE; 

                // Keep track of the USB interface STATE. 
                enum USBState
                {
                        BUS_INITIAL_STATE = 0, // Completely unconfigured, CPU reset state (atmega32u4, pg. 260)
                        BUS_INVALID_STATE = 1, // Bad State, must disable interface and restart 
                        BUS_UNPOWERED_STATE = 2, // Controller is Idle, VBUS flag is low 
                        BUS_ATTACHED_STATE = 3, // Controller is Idle, VBUS flag is high 
                        BUS_EOR_STATE = 4, // Device reset by host 
                        BUS_DEFAULT_STATE = 5, // Device recognized by host and ready to respond to CTL requests
                        
                };

                // stores the descriptor data 
                struct __attribute__((packed)) USB_DeviceDescriptor_t
                {
                        uint8_t bLength;
                        uint8_t bDescriptorType;
                        uint16_t bcdUSB;
                        uint8_t bDeviceClass;
                        uint8_t bDeviceSubClass;
                        uint8_t bDeviceProtocol;
                        uint8_t bMaxPacketSize0;
                        uint16_t idVendor;
                        uint16_t idProduct;
                        uint16_t bcdDevice;
                        uint8_t iManufacturer;
                        uint8_t iProduct;
                        uint8_t iSerialNumber;
                        uint8_t bNumConfigurations; 
                };

                struct __attribute__((packed)) USB_ConfigurationDescriptor_t
                {
                        uint8_t bLength;
                        uint8_t bDescriptorType;
                        uint16_t wTotalLength;
                        uint8_t bNumInterfaces;
                        uint8_t bConfigurationValue;
                        uint8_t iConfiguration;
                        uint8_t bmAttributes;
                        uint8_t bMaxPower;
                };

                struct __attribute__((packed)) USB_InterfaceDescriptor_t
                {
                        uint8_t bLength;
                        uint8_t bDescriptorType;
                        uint8_t bInterfaceNumber;
                        uint8_t bAlternateSetting;
                        uint8_t bNumEndpoints;
                        uint8_t bInterfaceClass;
                        uint8_t bInterfaceSubClass;
                        uint8_t bInterfaceProtocol;
                        uint8_t iInterface;
                };

                struct __attribute__((packed)) USB_EndpointDescriptor_t
                {
                        uint8_t bLength;
                        uint8_t bDescriptorType;	
                        uint8_t bEndpointAddress;
                        uint8_t bmAttributes;
                        uint16_t wMaxPacketSize;
                        uint8_t bInterval;
                };

                struct __attribute__((packed)) USB_Configuration_t
                {
                        USB_ConfigurationDescriptor_t ConfigurationDescriptor;
                        USB_InterfaceDescriptor_t InterfaceDescriptor;
                        USB_EndpointDescriptor_t InEndpointDescriptor; 
                        USB_EndpointDescriptor_t OutEndpointDescriptor; 
                };

                struct SetupPacket_t 
                {
                        uint8_t bmRequestType;
                        uint8_t bRequest;
                        uint16_t wValue; 
                        uint16_t wIndex; 
                        uint16_t wLength; 
                };



                static const USB_DeviceDescriptor_t DeviceDescriptor PROGMEM; 
                //static const USB_ConfigurationDescriptor_t ConfigDescriptor PROGMEM; 
                static const USB_Configuration_t Configuration PROGMEM; 

};
/* -------------------------------------------------------------------------- */

// USB ISRs 
// -------------------
ISR(USB_COM_vect); 
ISR(USB_GEN_vect); 
// ------------------


// Allow Global Serial Object to be referenced anywhere
//------------------------
extern SerialClass Serial; 
//------------------------

#endif