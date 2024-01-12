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

                // Overridden functions from interface
                uint_fast8_t read() override; 
                void readBytes(void * const data, const uint8_t len) override; 
                void write(const uint_fast8_t data) override;
                void writeBytes(const void * const data, uint8_t len) override; 
                void flushTX() override; 
                void flushRX() override; 

                inline void ISR_general(); 

                inline void ISR_common();

                uint8_t available(); 

        protected: 
                /* -------------- VARIABLES AND DATA STRUCTURES ------------- */
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
                
                struct __attribute__((packed)) USB_InterfaceAssociationDescriptor_t
                {
                        uint8_t bLength; 
                        uint8_t bDescriptorType;
                        uint8_t bFirstInterface;
                        uint8_t bInterfaceCount;
                        uint8_t bFunctionClass;
                        uint8_t bFunctionSubClass; 
                        uint8_t bFunctionProtocol; 
                        uint8_t iFunction; 
                };

                struct __attribute__((packed)) CDC_HeaderFunctionalDescriptor_t
                {
                        uint8_t bFunctionLength;
                        uint8_t bDescriptorType;
                        uint8_t bDescriptorSubtype;
                        uint16_t bcdCDC;
                }; 

                struct __attribute__((packed)) CDC_CallManagementFunctionalDescriptor_t
                {
                        uint8_t bFunctionLength;
                        uint8_t bDescriptorType;
                        uint8_t bDescriptorSubtype; 
                        uint8_t bmCapabilities; 
                        uint8_t bDataInterface; 
                };

                struct __attribute__((packed)) CDC_ACMControlManagementFunctionalDescriptor_t
                {
                        uint8_t bFunctionLength;
                        uint8_t bDescriptorType;
                        uint8_t bDescriptorSubtype; 
                        uint8_t bmCapabilities; 
                };

                struct __attribute__((packed)) CDC_UnionFunctionalDescriptor_t
                {
                        uint8_t bFunctionLength;
                        uint8_t bDescriptorType;
                        uint8_t bDescriptorSubtype;
                        uint8_t bMasterInterface;
                        uint8_t bSlaveInterface0; 
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
                        USB_InterfaceAssociationDescriptor_t InterfaceAssociationDescriptor;
                        USB_InterfaceDescriptor_t ACMInterfaceDescriptor;
                        CDC_HeaderFunctionalDescriptor_t HeaderFunctionalDescriptor;
                        CDC_CallManagementFunctionalDescriptor_t CallManagementFunctionalDescriptor; 
                        CDC_ACMControlManagementFunctionalDescriptor_t ACMControlManagementFunctionalDescriptor; 
                        CDC_UnionFunctionalDescriptor_t UnionFunctionalDescriptor; 
                        USB_EndpointDescriptor_t ACMEndpointDescriptor; 
                        USB_InterfaceDescriptor_t DataInterfaceDescriptor;
                        USB_EndpointDescriptor_t OutEndpointDescriptor; 
                        USB_EndpointDescriptor_t InEndpointDescriptor; 
                };
                
                struct SetupPacket_t 
                {
                        uint8_t bmRequestType;
                        uint8_t bRequest;
                        uint16_t wValue; 
                        uint16_t wIndex; 
                        uint16_t wLength; 
                };

                struct LineInfo_t
                {
                        uint32_t dwDTERate; 
                        uint8_t bCharFormat;
                        uint8_t bParityType;
                        uint8_t bDataBits;
                        uint8_t lineState; 
                };
                /* ---------------------------------------------------------- */

                /* --------------------- ENUM_CONSTANTS --------------------- */
                // Endpoint Numbering
                enum EP_NUM : uint8_t 
                {
                        EP_CTL_NUM = 0, // Endpoint 0 is always control
                        EP_ACM_NUM = 1, // Endpoint 1 is ACM endpoint
                        EP_RX_NUM = 2, // Endpoint 2 recieves data from PC 
                        EP_TX_NUM = 3, // Endpoint 3 sends data to PC 
                };

                // Endpoint Types (UECFG0X)
                enum EP_TYPE : uint8_t
                {
                        EP_TYPE_CTL = ((0 << EPTYPE1) | (0 << EPTYPE0) | (0 << EPDIR)),
                        EP_TYPE_BLK_IN = ((1 << EPTYPE1) | (0 << EPTYPE0) | (1 << EPDIR)),
                        EP_TYPE_BLK_OUT = ((1 << EPTYPE1) | (0 << EPTYPE0) | (0 << EPDIR)),
                        EP_TYPE_INT_IN = ((1 << EPTYPE1) | (1 << EPTYPE0) | (1 << EPDIR))
                };

                // Endpoint Size and Banks (UECFG1X)
                enum EP_SIZE : uint8_t 
                {
                        EP_64_BYTE_MASK = ((0 << EPSIZE2) | (1 << EPSIZE1) | (1 << EPSIZE0)),
                        EP_1_BANK_MASK = ((0 << EPBK1) | (0 << EPBK0)),
                        EP_2_BANK_MASK = ((0 << EPBK1) | (1 << EPBK0))
                };

                // Standard USB CTL EP bmRequestType Code Mask (usb_20.pdf pg. 248)
                enum USB_CTL_BMREQUESTTYPE : uint8_t
                {
                        D7_DIR_MASK = (1 << 7),
                        D7_DIR_HOST_TO_DEVICE_MASK = (0 << 7),
                        D7_DIR_DEVICE_TO_HOST_MASK = (1 << 7), 
                        D65_TYPE_MASK = ((1 << 6) | (1 << 5)),
                        D65_TYPE_STANDARD_MASK = 0x00,
                        D65_TYPE_CLASS_MASK = 0x20,
                        D65_TYPE_VENDOR_MASK = 0x60,
                        D40_RECIPIENT_MASK = ((1 << 4) | (1 << 3) | (1 << 2) | 
                                        (1 << 1) | (1 << 0)), //4 & 3 are reserved 
                        D40_RECIPIENT_DEVICE_MASK = 0x00,
                        D40_RECIPIENT_INTERFACE_MASK = 0x01, 
                        D40_RECIPIENT_ENDPOINT_MASK = 0x02,
                        D40_RECIPIENT_OTHER_MASK = 0x03
                };

                // Standard USB CTL EP bRequest Codes (usb_20.pdf pg. 251)
                enum USB_CTL_BREQUEST : uint8_t 
                {
                        GET_STATUS_REQ = 0,
                        CLEAR_FEATURE_REQ = 1,
                        SET_FEATURE_REQ = 3, 
                        SET_ADDRESS_REQ = 5,
                        GET_DESCRIPTOR_REQ = 6,
                        SET_DESCRIPTOR_REQ = 7,
                        GET_CONFIGURATION_REQ = 8,
                        SET_CONFIGURATION_REQ = 9,
                        GET_INTERFACE_REQ = 10, 
                        SET_INTERFACE_REQ = 11,
                        SYNCH_FRAME_REQ = 12
                };

                // Communication Device Class (CDC) Class bRequest Codes (AN758.pdf pg. 2)
                enum CDC_BREQUEST : uint8_t
                {
                        CDC_SET_LINE_CODING_REQ = 0x20,
                        CDC_GET_LINE_CODING_REQ = 0x21,
                        CDC_SET_CONTROL_LINE_STATE_REQ = 0x22,
                        CDC_SEND_BREAK = 0x23,
                        MSC_RESET = 0xFF,
                        MSC_GET_MAX_LUN = 0xFE
                };

                // Standard USB Descriptor Type Values (usb_20.pdf pg. 251)
                enum USB_DESCRIPTOR_TYPE : uint8_t 
                {
                        DESCRIPTOR_TYPE_DEVICE = 1,
                        DESCRIPTOR_TYPE_CONFIGURATION = 2,
                        DESCRIPTOR_TYPE_STRING = 3,
                        DESCRIPTOR_TYPE_INTERFACE = 4, 
                        DESCRIPTOR_TYPE_ENDPOINT = 5
                }; 

                // Standard USB Feature Selector Values (usb_20.pdf pg. 252)
                enum USB_FEATURE_SELECTOR : uint8_t
                {
                        FEATURE_DEVICE_REMOTE_WAKEUP = 1
                };

                // Standard USB GetStatus() Request Information (usb_20.pdf pg. 255)
                enum USB_GETSTATUS : uint8_t 
                {
                        D0_SELF_POWERED_MASK = (1 << 0),
                        D1_REMOTE_WAKEUP_MASK = (1 << 1)
                };

                // Endpoint Configuration Registers
                enum EP_CFG0 : uint8_t 
                {
                        EP_CTL_CFG0 = EP_TYPE_CTL,
                        EP_ACM_CFG0 = EP_TYPE_INT_IN,
                        EP_RX_CFG0 = EP_TYPE_BLK_OUT,
                        EP_TX_CFG0 = EP_TYPE_BLK_IN
                };

                enum EP_CFG1 : uint8_t 
                {
                        EP_CTL_CFG1 = (EP_64_BYTE_MASK | EP_1_BANK_MASK | (1 << ALLOC)),
                        EP_ACM_CFG1 = (EP_64_BYTE_MASK | EP_2_BANK_MASK | (1 << ALLOC)),
                        EP_RX_CFG1 = (EP_64_BYTE_MASK | EP_2_BANK_MASK | (1 << ALLOC)),
                        EP_TX_CFG1 = (EP_64_BYTE_MASK | EP_2_BANK_MASK | (1 << ALLOC))
                };
                /* ---------------------------------------------------------- */

                /* ------------------- PRORTECTED_METHODS ------------------- */
                // Desc: Initializes the USB interface 
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
                void enableUSBCLK();

                // Desc: Freezes the USB CLK for power savings 
                // Args: None 
                // Returns: None 
                void disableUSBCLK(); 

                // Desc: Configures the endpoint using the specified register values
                // Args: None
                // Returns: None 
                void initEP(const uint8_t epNum, 
                        const uint8_t epCFG0, const uint8_t epCFG1); 
                
                // Desc: Reads a byte from the currently selected EP's fifo
                // Args: None
                // Returns: None 
                inline uint8_t rx8(); 

                // Desc: Puts a byte into the currently selected EP's fifo 
                // Args: None
                // Returns: None 
                inline void tx8(const uint8_t data);

                // Desc: Clears the RX (OUT) buffer and discards any contents, also switches banks
                // Args: None 
                // Returns: Nothing 
                inline void releaseRX(); 
        
                // Desc: Sends the TX (IN) buffer and switches to the other bank 
                // Args: None
                // Returns: Nothing 
                inline void releaseTX(); 

                // Desc: Sets the selected endpoint number
                // Args: epNum: Value to set UENUM to 
                // Returns: Nothing 
                inline void setEP(const uint8_t epNum); 

                // Desc: Returns the byte count of the selected endpoint's FIFO
                //      TX (IN): Storing in fifo -> increases, sending from fifo -> decreases
                //      RX (OUT): Receive to fifo -> increases, read out of fifo -> decreases
                // Args: None
                // Returns: Byte Representing LSB of FIFO's current bank count 
                inline uint8_t fifoByteCount(); 

                // Desc: Checks whether the FIFO can be read or written to 
                //      TX (IN): True if current bank is not full
                //      RX (OUT): True if unread data is in fifo 
                // Args: None
                // Returns: Nothing 
                inline uint8_t isRWAllowed(); 
                
                // Desc: Used with the CTL endpoint, waits for the fifo to be ready to receive data to transmit 
                // Args: None
                // Returns: None 
                inline void waitForTxRdy();

                // Desc: Clears the TXINI bit so that it can be used to wait 
                // Args: None
                // Returns: Nothing
                inline void clrTxRdy();

                // Desc: Clears various flags so that they can be used for state changes
                // Args: None
                // Returns: Nothing
                inline void clrGenISRFlags(); 

                

                /* ---------------------------------------------------------- */



                inline uint_fast8_t waitForInOut(); 
                inline void stall(); 
                void InitOtherEP();
                inline void waitOut(); 


                inline uint_fast8_t isStalled(); 
                inline uint_fast8_t isFifoFree(); 



                uint_fast8_t sendSpace(const uint_fast8_t epNum); 

                uint_fast16_t send(uint_fast8_t epNum, const void * d, uint_fast16_t len); 

                void SendStringDescriptor(const void * const data, const uint8_t len);
                void sendProgMemPayload(const void * const dataPtr, const uint_fast8_t len, uint8_t maxLen); 
                void sendMemPayload(const void * const dataPtr, const uint_fast8_t len, uint8_t maxLen);

                void receiveControl(void * d, uint16_t len); 
                void receive(uint8_t epNum, void * d, uint8_t len); 


                volatile uint8_t usbConfiguration = 0;  
                volatile uint8_t currentStatus = 0; // initial status (no remote wakeup, bus powered)
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

                static const USB_DeviceDescriptor_t DeviceDescriptor PROGMEM; 
                //static const USB_ConfigurationDescriptor_t ConfigDescriptor PROGMEM; 
                static const USB_Configuration_t Configuration PROGMEM; 
                
                volatile LineInfo_t usbLineInfo = 
                {
                        57600,
                        0x00,
                        0x00,
                        0x00,
                        0x00
                }; 
                

                // handle class requests 
                inline uint8_t classInterfaceRequest(SetupPacket_t &setup); 
                inline uint8_t CDC_Setup(SetupPacket_t &setup);
                int32_t breakValue = -1; 

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