/*
This file contains various defines that hold constants for the CDC serial interface.
*/

#ifndef CDCSERIALDEFINES_H
#define CDCSERIALDEFINES_H

/* ------------------------------ CONFIGURATION ----------------------------- */
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

// Construct EP UECFG0X values from parameters
#define EP_CTL_UECFG0X (EP_TYPE_CTL_MASK)
#define EP_ACM_
/* -------------------------------------------------------------------------- */
#endif