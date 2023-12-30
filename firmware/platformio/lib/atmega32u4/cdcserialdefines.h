/*
This file contains various defines that hold constants for the CDC serial interface.
*/

#ifndef CDCSERIALDEFINES_H
#define CDCSERIALDEFINES_H

/* ------------------------- CONFIGURATION_CONSTANTS ------------------------ */
// USB Powersaving PLL Lock ID, (one of 8 possible unique id's)
#define USB_PLL_LOCK_ID (1 << 0) //id 0 

// Endpoint Numbering
#define EP_CTL_NUM 0 // Endpoint 0 is always control 
#define EP_ACM_NUM 1 // Endpoint 1 is ACM endpoint 
#define EP_RX_NUM 2 // Endpoint 2 receives data from PC
#define EP_TX_NUM 3 // Endpoint 3 sends data to PC

// Endpoint Types (UECFG0X Mask)
#define EP_TYPE_CTL_MASK ((0 << EPTYPE1) | (0 << EPTYPE0) | (0 << EPDIR))
#define EP_TYPE_ISO_IN_MASK ((0 << EPTYPE1) | (1 << EPTYPE0) | (1 << EPDIR))
#define EP_TYPE_ISO_OUT_MASK ((0 << EPTYPE1) | (1 << EPTYPE0) | (0 << EPDIR))
#define EP_TYPE_BLK_IN_MASK ((1 << EPTYPE1) | (0 << EPTYPE0) | (1 << EPDIR))
#define EP_TYPE_BLK_OUT_MASK ((1 << EPTYPE1) | (0 << EPTYPE0) | (0 << EPDIR))
#define EP_TYPE_INT_IN_MASK ((1 << EPTYPE1) | (1 << EPTYPE0) | (1 << EPDIR))
#define EP_TYPE_INT_OUT_MASK ((1 << EPTYPE1) | (1 << EPTYPE0) | (0 << EPDIR))

// EndPoint Size and Banks (UECFG1X Mask)
#define EP_16_BYTE_MASK ((0 << EPSIZE2) | (0 << EPSIZE1) | (1 << EPSIZE0))
#define EP_64_BYTE_MASK ((0 << EPSIZE2) | (1 << EPSIZE1) | (1 << EPSIZE0))
#define EP_1_BANK_MASK ((0 << EPBK1) | (0 << EPBK0))
#define EP_2_BANK_MASK ((0 << EPBK1) | (1 << EPBK0))

// Standard USB CTL EP bmRequestType Code Mask (usb_20.pdf pg. 248)
#define D7_DIR_MASK ((1 << 7))
#define D7_DIR_HOST_TO_DEVICE_MASK 0x00
#define D7_DIR_DEVICE_TO_HOST_MASK 0x80
#define D65_TYPE_MASK ((1 << 6) | (1 << 5))
#define D65_TYPE_STANDARD_MASK 0x00
#define D65_TYPE_CLASS_MASK 0x20
#define D65_TYPE_VENDOR_MASK 0x60
#define D40_RECIPIENT_MASK ((1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))
#define D40_RECIPIENT_DEVICE_MASK 0x00
#define D40_RECIPIENT_INTERFACE_MASK 0x01
#define D40_RECIPIENT_ENDPOINT_MASK 0x02
#define D40_RECIPIENT_OTHER_MASK 0x03

// Standard USB CTL EP bRequest Codes (usb_20.pdf pg. 251)
#define GET_STATUS_REQ 0
#define CLEAR_FEATURE_REQ 1
#define SET_FEATURE_REQ 3
#define SET_ADDRESS_REQ 5
#define GET_DESCRIPTOR_REQ 6
#define SET_DESCRIPTOR_REQ 7
#define GET_CONFIGURATION_REQ 8
#define SET_CONFIGURATION_REQ 9
#define GET_INTERFACE_REQ 10
#define SET_INTERFACE_REQ 11
#define SYNCH_FRAME_REQ 12

// Communication Device Class (CDC) Class Request Codes (AN758.pdf pg. 2)
#define CDC_SET_LINE_CODING_REQ 0x20 // configures baud, stop-bits, parity, and number of characters
#define CDC_GET_LINE_CODING_REQ 0x21 // requests DTE rate, stop-bite, parity, and number of characters
#define CDC_SET_CONTROL_LINE_STATE_REQ 0x22 // 'RS232' used to tell DCE device the DTE device is present 
#define CDC_SEND_BREAK 0x23 // not in AN758 but in USBCore.h 
#define MSC_RESET 0xFF // not in AN758 but in USBCore.h 
#define MSC_GET_MAX_LUN 0xFE // not in AN758 but in USBCore.h 

// Standard USB Descriptor Type Values (usb_20.pdf pg. 251)
#define DESCRIPTOR_TYPE_DEVICE 1
#define DESCRIPTOR_TYPE_CONFIGURATION 2
#define DESCRIPTOR_TYPE_STRING 3
#define DESCRIPTOR_TYPE_INTERFACE 4
#define DESCRIPTOR_TYPE_ENDPOINT 5
#define DESCRIPTOR_TYPE_DEVICE_QUALIFIER 6
#define DESCRIPTOR_TYPE_OTHER_SPEED_CONFIG 7
#define DESCRIPTOR_TYPE_INTERFACE_POWER 8

// Standard USB Feature Selector Values (usb_20.pdf pg. 252)
#define FEATURE_DEVICE_REMOTE_WAKEUP 1
#define FEATURE_ENDPOINT_HALT 0
#define FEATURE_TEST_MODE 2

// Standard USB GetStatus() Request Information (usb_20.pdf pg. 255)
#define D0_SELF_POWERED_MASK ((1 << 0))
#define D1_REMOTE_WAKEUP_MASK ((1 << 1))

// Endpoint 0 (Control) Constants
#define EP_CTL_CFG0 ((EP_TYPE_CTL_MASK))
#define EP_CTL_CFG1 ((EP_64_BYTE_MASK) | (EP_1_BANK_MASK) | (1 << ALLOC)) 

/* -------------------------------------------------------------------------- */
#endif