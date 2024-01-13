#include "cdcserial.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "time.h"
#include <util/atomic.h>
#include "lightdebug.h"
#include "powersaving.h"
#include <avr/pgmspace.h> //PROGMEM
#include <util/delay.h>

using namespace std; 

/* ------------------------ SERIALCLASS_IMPLMENTATION ----------------------- */
SerialClass::SerialClass()
{
        // TODO: REMOVE DEBUG LIGHT
        redOff(); 
        greenOff();
        blueOff(); 
        yellowOff(); 
        print(0); 

        // Initialize the USB system and registers
        initUSB(); 
}

uint_fast8_t SerialClass::read()
{
        uint8_t temp; 
        readBytes(&temp, 1); 
        return temp; 
}

void SerialClass::readBytes(void * const data, uint8_t len) 
{
        // TODO: Does the USB configuration need to be checked? 

        // Convert the void ptr to a uint8_t ptr for bytewise access
        uint8_t * dataPtr = reinterpret_cast<uint8_t *>(data);

        // To prevent interrupts changing EpNum, put in atomic block
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                // Select the RX Endpoint 
                setEP(EP_RX_NUM); 

                // Loop len times to read requested bytes
                while (len--)
                {
                        // Read a single byte from the RX FIFO 
                        (*(dataPtr++)) = rx8();
                }

                if (!fifoByteCount()) {
                        // if fifoByteCount is 0, then current bank is empty, 
                        //      release it so it can be filled by Host 
                        releaseRX(); 
                }
        }
        return;
}

void SerialClass::write(const uint_fast8_t data)
{
        //! For now implemented with writeBytes, may optimize later
        writeBytes(&data, 1);
        return; 
}

void SerialClass::writeBytes(const void * const data, uint8_t len)
{
        // This function only releases the current bank if it needs to, flushTX should be
        //      called following a write sequence. 

        // TODO: Does the USB configuration need to be checked?

        // TODO: Does the Suspend State need to be checked?

        // Convert the void ptr to a uint8_t ptr for bytewise access
        const uint8_t * dataPtr = reinterpret_cast<const uint8_t *>(data);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                // Select the TX (IN) Endpoint
                setEP(EP_TX_NUM);

                // keep looping until remaining length is 0
                while (len)
                {
                        // How much room is in FIFO 
                        uint8_t fifoSpace = 64 - fifoByteCount(); 

                        if (!fifoSpace) { 
                                // if there is no room in FIFO, release the buffer
                                releaseTX(); 
                                
                                // wait for the bank switch to occur
                                while (!isRWAllowed()) { ; }

                                // bank has changed, recalculate fifoSpace
                                fifoSpace = 64 - fifoByteCount();
                                }

                        // send the most bytes possible this iteration
                        uint8_t n = (len < fifoSpace) ? (len) : (fifoSpace);
                        len -= n; 
                        while (n--)
                        {
                                // Send a byte and increment the pointer
                                tx8(*(dataPtr++)); 
                        }
                }
        }
        return; 
}

void SerialClass::flushTX()
{
        // If any bytes are remaining in the TX (IN) FIFO send them
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                // select the TX (IN) Endpoint
                setEP(EP_TX_NUM); 

                //  check to see if there are unsent bytes
                uint8_t fifoCount = fifoByteCount();
                if (fifoCount) {
                        // releaseTX bank to send the bytes
                        releaseTX();

                        // If the last packet was a max length of 64 Bytes, a zero length packet (ZLP)
                        //      must be sent to signal that end of data has been reached
                        // Check to see if the FIFO was full 
                        if (fifoCount == 64) {
                                // wait for the prior bank switch to occur
                                while (!isRWAllowed()) { ; }

                                // send the ZLP packet by not writing any data
                                releaseTX(); 
                        }
                }
        }
        return; 
}

void SerialClass::flushRX()
{
        // Do not need to worry about ZLPs, simply release the bank 
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                setEP(EP_RX_NUM);
                releaseRX(); 
        }
        return; 
}

void SerialClass::initUSB()
{
        // Initialize USB interface. (atmega32u4, pg. 266)
        // Initialization process is based off of Arduino CDC Serial Code
        // Additional Information is from here: 
        // https://kampi.gitbook.io/avr/lets-use-usb/initialize-the-usb-as-a-usb-device
        // https://www.usbmadesimple.co.uk/ums_4.htm
        // http://janaxelson.com/usbcenum.htm

        // In this function, the following actions will be performed
        // 1. Configure the PLL (configure not enable)
        // 2. Reset the USB controller to idle state
        // 3. Configure the USB regulator 
        // 4. Enable VBUS interrupts
        // 5. Update state variable 
        // 5A. If VBUS is high, configure endpoints and attach device
        // 5B. If VBUS is low, VBUS interrupt will handle connection 
        // 6A. Clear VBUS int to guarantee that it will not be called again  

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
                // EORSTE: When host resets USB device duiring initialization set the EORSTI flag (atmega32u4, pg. 283)
                UDIEN |= (1 << EORSTE); 
                
                // If VBUS is already high, enable the PLL and unfreeze the clock 
                // If VBUS is not currently high, then the clock will enable when 
                // the VBUSI interrupt is triggered //TODO: IMPLEMENT THIS 
                if (USBSTA & (1 << VBUS)) {
                        // If initially powered, go ahead and try to connect
                        // Otherwise the connection will be handled by VBUS interrupt 
                        enableUSBCLK(); 

                        // Attach the device so that the host starts reset (atmega32u4, pg. 281)
                        UDCON &= ~(1 << DETACH);

                        // Clear the VBUSTI flag so that interrupt is not called again (atmega32u4, pg. 268)
                        USBINT &= ~(1 << VBUSTI); 

                }
        }
        
        // PLL activated in the ISR  
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

void SerialClass::enableUSBCLK()
{
        // To enable clock lock the PLL so it is not disabled by powersaving
        // Using id position of 0
        Pwr.lockPLL(PowerSaving::USB_PLL_LOCK_ID); 

        // Now that PLL is enabled, unfreeze the USB clock (atmega32u4, pg. 267)
        USBCON &= ~(1 << FRZCLK);
        return; 
}

void SerialClass::disableUSBCLK()
{
        // freeze the USB clock (atmega32u4, pg.267)
        USBCON |= (1 << FRZCLK); 

        // unlock the PLL so that powersaving may disable it
        Pwr.unlockPLL(PowerSaving::USB_PLL_LOCK_ID);
        return; 
}

void SerialClass::initEP(const uint8_t epNum, 
        const uint8_t epCFG0, const uint8_t epCFG1)
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
        /*
        if (!(UESTA0X & (1 << CFGOK))) {
                // Invalid State, mark the bus to be reset 
                state = BUS_INVALID_STATE; 
        } //! Assuming that all init is successful 
        */ 
        return; 
}

inline uint8_t SerialClass::rx8()
{
        return UEDATX; 
}

inline void SerialClass::tx8(const uint8_t data)
{
        UEDATX = data; 
        return; 
}

inline void SerialClass::waitForTxRdy()
{
        // wait until the TXINI flag is set, signifying the current bank if free (atmega32u4, pg. 290)
        // ^ since the RWAL bit cannot be used for CTL EP 
        while (!(UEINTX & (1 << TXINI))) { ; }
        return; 
}

inline void SerialClass::clrTxRdy()
{
        // Clear the TXINI bit (atmega32u4, pg. 290)
        UEINTX &= ~(1 << TXINI);
        return; 
}

inline void SerialClass::clrGenISRFlags()
{
        // Clear Setup Packet Interrupt (atmega32u4, pg. 290)
        // Also clear RXOUTI and TXINI so they can be used to lock registers until ready 
        UEINTX &= ~((1 << RXSTPI) | (1 << TXINI) | (1 << RXOUTI));
        return; 
}

inline uint_fast8_t SerialClass::waitForInOut()
{
        // wait for either a byte to be received or the output buffer to be empty 
        while (!(UEINTX & ((1 << RXOUTI) | (1 << TXINI)))) { ; }

        // If byte received, raise error 
        return (!(UEINTX & (1 << RXOUTI))); // true is no error, false is error
}

inline void SerialClass::stall()
{
        // Setting STALLRQ -> Requests stall from host (atmega32u4, pg. 286)
        // 
        UECONX |= (1<<STALLRQ);
        return; 
}

void SerialClass::sendProgMemPayload(const void * const dataPtr, const uint_fast8_t len, uint8_t maxLen)
{
        // Send 'len' number of bytes starting from dataPtr address 
        // Sent byte by byte so convert void ptr to byte pointer 
        const uint8_t * dataRunPtr = reinterpret_cast<const uint8_t *>(dataPtr);  
        uint8_t numBytesSent = 0; 
        
        for (uint_fast8_t i = 0; i < len; i++)
        {
                // Read a byte of the descriptor struct from flash 
                uint8_t dataByte = pgm_read_byte(dataRunPtr++); 

                // Wait for the FIFO to be ready for next packet, if RX received error 
                if (!waitForInOut()) { 
                        break;  
                        } // TODO: ERROR

                // Send the byte 
                tx8(dataByte); // TODO: Keep track of how many bytes in CTL EP
                maxLen--; 
                numBytesSent++; // TODO: DEBUG JANKNESS

                if (!maxLen){ // TODO: THIS MORE ELEGANT 
                        // don't send anymore bytes
                        break; 
                }

                if (numBytesSent == 64) {
                        // Start Transmitting the bytes since the buffer is full 
                        UEINTX &= ~static_cast<uint8_t>((1<<TXINI));
                }
        }
        return; 
}

void SerialClass::sendMemPayload(const void * const dataPtr, const uint_fast8_t len, uint8_t maxLen)
{
        // Send 'len' number of bytes starting from dataPtr address 
        // Sent byte by byte so convert void ptr to byte pointer 
        const uint8_t * dataRunPtr = reinterpret_cast<const uint8_t *>(dataPtr);  
        uint8_t numBytesSent = 0; 
        
        for (uint_fast8_t i = 0; i < len; i++)
        {
                // Read a byte of the descriptor struct from flash 
                uint8_t dataByte = *(dataRunPtr++); 

                // Wait for the FIFO to be ready for next packet, if RX received error 
                if (!waitForInOut()) { 
                        break;  
                        } // TODO: ERROR  

                // Send the byte 
                tx8(dataByte); // TODO: Keep track of how many bytes in CTL EP
                maxLen--; 
                numBytesSent++; // TODO: DEBUG JANKNESS

                if (!maxLen){ // TODO: THIS MORE ELEGANT 
                        // don't send anymore bytes
                        break; 
                }

                if (numBytesSent == 64) {
                        // Start Transmitting the bytes since the buffer is full 
                        UEINTX &= ~static_cast<uint8_t>((1<<TXINI));
                }
        }
        return; 
}

void SerialClass::SendStringDescriptor(const void * const data, const uint8_t len)
{
        // send string descriptor format (atmega32u4, pg. 274) 
        uint8_t temp = 2+len*2;
        sendMemPayload(&temp, 1, 63); // .bLength 
        temp = 3; 
        sendMemPayload(&temp, 1, 63); // .bDescriptorType 
        sendProgMemPayload(data, len, 64);
        return; 
}

void SerialClass::InitOtherEP()
{
        // initialize needed EP's
        initEP(EP_ACM_NUM, EP_ACM_CFG0, EP_ACM_CFG1);
        initEP(EP_RX_NUM, EP_RX_CFG0, EP_RX_CFG1);
        initEP(EP_TX_NUM, EP_TX_CFG0, EP_TX_CFG1);

        // To use EP's reset them and then clear RST flags (atmega32, pg. 285)
        UERST = 0x7E; // don't reset EP0 
        UERST = 0x00; 
        return; 
}

inline uint8_t SerialClass::fifoByteCount()
{
        // Low Byte of selected EP FIFO Byte Count (atmega32u4, pg. 291)
        // Since 64 is the max size, only low byte is needed 
        // IN endpoint (TX): incremented after writing 
        // OUT endpoint (RX): incremented by receiving from host 
        return UEBCLX; 
}

inline uint8_t SerialClass::isRWAllowed()
{
        // Set when reading or writing is valid (atmega32u4, pg. 289)
        // IN Endpoint: the current bank's fifo is not full
        // OUT Endpoint: the current bank is not empty, unread data 
        return (UEINTX & (1 << RWAL)); 
}

inline uint_fast8_t SerialClass::isStalled()
{
        // set when a stall handshake has been sent (atmega32u4, pg. 290)
        return (UEINTX & (1 << STALLEDI));
}

inline uint_fast8_t SerialClass::isFifoFree()
{
        // (atmega32u4, pg. 289)
        return (UEINTX & (1 << FIFOCON)); 
}

inline void SerialClass::releaseRX()
{
        // To speed up this function we can explicitly define all bits in the UEINTX register
        // This way an OR operation will not need to be performed 
        // For releasing the RX -> OUT Endpoint (atmega32u4, pg. 289)
        // FIFOCON: 0 -> Switches to the next bank 
        // NAKINI: 1 -> Setting by software has no effect 
        // RWAL: 1 -> Not set by software, should have no impact
        // NAKOUTI: 1 -> Setting by software has no effect 
        // RXSTPI: 1 -> Settting by software has no effect 
        // RXOUTI: 0 -> Setting activates KILLBK, clear instead
        // STALLEDI: 1 -> Setting by software has no effect 
        // TXINI: 1 -> Setting by software has no effect 
        UEINTX = ((0 << FIFOCON) | (1 << NAKINI) | (1 << RWAL) | (1 << NAKOUTI) |
                (1 << RXSTPI) | (0 << RXOUTI) | (1 << STALLEDI) | (1 << TXINI));
        return; 
}

inline void SerialClass::releaseTX()
{
        // To speed up this function we can explicitly define all bits in the UEINTX register
        // This way an OR operation will not need to be performed 
        // For releasing the TX -> IN Endpoint (atmega32u4, pg. 289)
        // FIFOCON: 0 -> Switches to the next bank and sends data
        // NAKINI: 1 -> Setting by software has no effect 
        // RWAL: 1 -> Not set by software, should have no impact
        // NAKOUTI: 1 -> Setting by software has no effect 
        // RXSTPI: 1 -> Settting by software has no effect 
        // RXOUTI: 0 -> Setting activates KILLBK, clear instead
        // STALLEDI: 1 -> Setting by software has no effect 
        // TXINI: 0 -> Handshake the INT and clear 
        UEINTX = ((0 << FIFOCON) | (1 << NAKINI) | (1 << RWAL) | (1 << NAKOUTI) |
                (1 << RXSTPI) | (0 << RXOUTI) | (1 << STALLEDI) | (0 << TXINI));
        //UEINTX = 0x3A;
        return; 
}

inline void SerialClass::setEP(const uint8_t epNum)
{
        UENUM = epNum;
        return; 
}

uint_fast8_t SerialClass::sendSpace(const uint_fast8_t epNum)
{
        uint_fast8_t retVal; 
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                UENUM = epNum;
                if (!isRWAllowed()) {
                        retVal = 0; 
                } else {
                        retVal = 64 - fifoByteCount(); 
                }
        }
        return retVal; 
}

uint_fast16_t SerialClass::send(uint_fast8_t epNum, const void * d, uint_fast16_t len)
{ 
        if (!usbConfiguration) {
                return 0; // usb is not fully configured yet 
        }

        if (UDINT & (1 << SUSPI)){
                // if true the interface is suspended, send a wakeup to the host 
                // trigger remote wakeup request 
                UDCON |= (1 << RMWKUP);
        }

        uint_fast16_t r = len;
        const uint8_t * data = reinterpret_cast<const uint8_t *>(d); 
        uint8_t timeout = 250; 
        bool sendZlp = false; 
        while (len || sendZlp)
        {
                uint8_t n = sendSpace(epNum); 
                if (n == 0) {
                        if (!(--timeout)) { return 0; }
                        Time.delayMs(1); 
                        continue; 
                        // if no space is available wait for up to 250ms per call
                }
                if (n > len) { //! CHECK TO SEE IF THERE IS AN ISSUE WHEN EXACTLY 64 BYTES ARE TRANSMITTED, NEED TO SEND A ZLP FOR CDC?  
                        n = len; // space is good to go ahead and send 
                }

                ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                {
                        UENUM = epNum; 
                        //make sure interrupt hasn't released frames 
                        if (!isRWAllowed()) {
                                continue; 
                        }

                        len -= n; 
                        if (epNum & 0x20) { // this is dumb change this implementation
                                while (n--) 
                                {
                                        tx8(0); // send all the bytes there is room for 
                                }
                        } else if (epNum & 0x80) {
                                while (n--)  
                                {
                                        tx8(pgm_read_byte(data++));
                                }
                        } else {
                                while (n--)
                                {
                                        tx8(*(data++)); 
                                }
                        }

                        if (sendZlp) {
                                releaseTX(); // send ZLP 
                                sendZlp = false;  
                        } else if (!isRWAllowed()) {
                                releaseTX(); // The buffer is full, change the fifo and send the previous packet 
                                if (len == 0) {
                                        sendZlp = true; // if all bytes sent, must send a Zlp to signify end of data to send  //!: IS THIS ONLY NEEDED FOR 64 BYTE MULTIPLE PACKETS? 
                                }
                        } // not implementing transfer release
                        //releaseTX(); //TODO: better solution for this? 
                }
        }
        return r; 
}

uint8_t SerialClass::available()
{
        uint8_t numAvail = 0; 
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                UENUM = EP_RX_NUM;
                numAvail = fifoByteCount();
                UENUM = EP_CTL_NUM; 
        }
        return numAvail; 
}

void SerialClass::receive(uint8_t epNum, void * d, uint8_t len)
{
        if (!usbConfiguration) { 
                return;
        }

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                UENUM = epNum; 
                uint8_t n = fifoByteCount(); 
                len = (n < len) ? (n) : (len); 
                n = len; 
                uint8_t * dst = (uint8_t *)d; 
                while (n--)
                {
                        *(dst++) = rx8(); 
                }
                if (len && !fifoByteCount()) { //release empty buffer if all have been read 
                        releaseRX(); 
                }
        }
        return; 
}

inline uint8_t SerialClass::classInterfaceRequest(SetupPacket_t &setup)
{
        uint8_t i = setup.wIndex; // represents interface number
        if (i == 0) {
                // ACM interface
                return CDC_Setup(setup); 
        }
        return true; 
}

inline uint8_t SerialClass::CDC_Setup(SetupPacket_t &setup)
{
        uint8_t r = setup.bRequest;
        uint8_t requestType = setup.bmRequestType; 

        if ((D7_DIR_DEVICE_TO_HOST_MASK | D65_TYPE_CLASS_MASK | D40_RECIPIENT_INTERFACE_MASK) == requestType) {
                if (CDC_GET_LINE_CODING_REQ == r) {
                        sendMemPayload((const void *)&usbLineInfo, 7, 64); 
                        return false; 
                }
        }
        if ((D7_DIR_HOST_TO_DEVICE_MASK | D65_TYPE_CLASS_MASK | D40_RECIPIENT_INTERFACE_MASK) == requestType) {
                if (CDC_SEND_BREAK == r) {
                        breakValue = setup.wValue; // TODO: ? 
                }

                if (CDC_SET_LINE_CODING_REQ == r) {
                        receiveControl((void*)&usbLineInfo, 7); 
                }

                if (CDC_SET_CONTROL_LINE_STATE_REQ == r) {
                        usbLineInfo.lineState = (setup.wValue & 0x00FF); 
                }
        }
        return false; 
}

inline void SerialClass::waitOut()
{
        while (!(UEINTX & (1<<RXOUTI))) { ; }
        return; 
}

void SerialClass::receiveControl(void * d, uint16_t len)
{
        uint16_t length = len; 
        uint8_t * dataPtr = reinterpret_cast<uint8_t *>(d); 
        while (length)
        {
                uint16_t recvLength = length; 
                if (recvLength > 64) {
                        recvLength = 64; 
                        // max of 64 bytes in fifo 
                }

                // fill the last portion of the data array
                waitOut(); //wait for there to be unread data 
                uint8_t count = recvLength;
                while (count--)
                {
                        *((dataPtr++) + len - length) = rx8(); 
                }
                UEINTX = static_cast<uint8_t>(~(1<<RXOUTI)); // CLEAROUT 
                length -= recvLength; 
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
                } else {
                        // VBUS is low 
                        // TODO: Proper Powersaving procedure

                        // Set interface to be detached
                        UDCON |= (1 << DETACH);
                        // Disable the Clock and unlock PLL
                        disableUSBCLK();
                }
        }

        // Handle a USB End of Reset Interrupt 
        // Device has been reset by host 
        if (UDINT & (1 << EORSTI)){
                // EOR Int occured
                // Clear the Flag 
                UDINT &= ~(1 << EORSTI); 

                // Configure Endpoint 0 
                initEP(EP_CTL_NUM, EP_CTL_CFG0, EP_CTL_CFG1); 
                                
                // Enable Setup Packet Detection Interrupts [host to device] (atmega32u4, pg. 291)
                UEIENX = (1 << RXSTPE); 
        }

        // Handle a Wakeup Interrupt (Non Idle Signals) (atmega32u4, pg. 272)
        if (UDINT & (1 << WAKEUPI)) {
                // Wakeup Interrupt already deteched, now disable WAKEUPI to save future cycles. 
                // Also enable the SUSPI interrupt to detect when the system will sleep 
                UDIEN = (UDIEN & ~(1<<WAKEUPE)) | (1<<SUSPE);

                // TODO: Powersaving Stuff
                redOn(); // signal usb is active

                // Clear WAKEUPI flag since it was just handled. 
                // Do not clear SUSPI since it only occurs once and we do not want to miss it (should not be set yet) 
                UDINT &= ~(1<<WAKEUPI);

        } else if (UDINT & (1 << SUSPI)) {
                // USB suspend interrupt detected
                // Disable the SUSPI interrupt and enable the WAKEUPI interrupt
                UDIEN = (UDIEN & ~(1<<SUSPE)) | (1<<WAKEUPE);
                
                // TODO: Powersaving Stuff
                redOff(); // signal usb is suspended

                // Clear interrupt flags to prevent the ISR from being triggered again
                // Since WAKEUPI can occur anytime, clear its flag to make sure a residual interrupt did not trigger a wakeup 
                UDINT &= ~((1<<WAKEUPI) | (1<<SUSPI));
        }
        return; 
}

inline void SerialClass::ISR_common()
{
        // ONLY EP0 INTERRUPTS ENABLED

        // Store selected EP and select EP0 
        setEP(EP_CTL_NUM); 

        // Read in the 8 byte setup packet (usb_20.pdf, pg. 248)
        SetupPacket_t setup; 
        uint8_t * setupRunPtr = reinterpret_cast<uint8_t *>(&setup); 
        for (uint_fast8_t i = 0; i < sizeof(SetupPacket_t); i++)
        {
                (*(setupRunPtr++)) = rx8();
        }

        // Clear interrupts to delete the EP0 bank (atmega32u4, pg. 274)
        clrGenISRFlags(); 
        
        // If we need to transmit, wait for the banks to be clear 
        if (setup.bmRequestType & D7_DIR_DEVICE_TO_HOST_MASK) {
                waitForTxRdy(); 
        } else {
                clrTxRdy(); // make sure the EP0 bank is empty
        }
        
        // Must wait to end to stall since during a stall a traditional packet is not sent
        uint8_t toStall = false; 

        // Carry out the Proper Action 
        switch (setup.bmRequestType & D65_TYPE_MASK)
        {
                case (D65_TYPE_STANDARD_MASK):
                        // Standard USB Request 
                        switch (setup.bRequest)
                        {
                                case (GET_STATUS_REQ):
                                        // Get Status Request (usb_20.pdf, pg. 254)
                                        // send two bytes according to Figure 9-4 (usb_20.pdf, pg. 255)
                                        if (setup.bmRequestType == (D7_DIR_DEVICE_TO_HOST_MASK | D65_TYPE_STANDARD_MASK | D40_RECIPIENT_DEVICE_MASK)) {
                                                // sent to device
                                                tx8(currentStatus); // status of remote wakeup and self powered fields
                                                tx8(0); // second byte is reserved. 
                                        } else {
                                                // if request is anything else pad with NULL (two 0's)
                                                // TODO: Halt state of endpoint?
                                                tx8(0); 
                                                tx8(0); 
                                        }
                                        break;
                                case (CLEAR_FEATURE_REQ):
                                        // Clear Feature Request (usb_20.pdf, pg. 252)
                                        if ((setup.bmRequestType == (D7_DIR_HOST_TO_DEVICE_MASK | D65_TYPE_STANDARD_MASK | D40_RECIPIENT_DEVICE_MASK)) 
                                                && (setup.wValue == FEATURE_DEVICE_REMOTE_WAKEUP)) {
                                                        // Disable Remote Wakeup Functionality 
                                                        currentStatus &= ~D1_REMOTE_WAKEUP_MASK; 
                                                }
                                        break;
                                case (SET_FEATURE_REQ):
                                        // Set Feature Request (usb_20.pdf, pg. 258) 
                                        if ((setup.bmRequestType == (D7_DIR_HOST_TO_DEVICE_MASK | D65_TYPE_STANDARD_MASK | D40_RECIPIENT_DEVICE_MASK))
                                                && (setup.wValue == FEATURE_DEVICE_REMOTE_WAKEUP)) {
                                                        // Enable Remote Wakeup Functionality 
                                                        currentStatus |= D1_REMOTE_WAKEUP_MASK;
                                                }
                                        break;
                                case (SET_ADDRESS_REQ):
                                        // Set Address Request (usb_20.pdf)
                                        // Wait for the device to finish sending the ACK 
                                        waitForTxRdy(); 
                                        // Set the received address and enable it (atmega32u4, pg. 272; pg. 284)
                                        // Should be done in separate steps 
                                        UDADDR = (setup.wValue & 0x7F); 
                                        UDADDR |= (1 << ADDEN);
                                        break;
                                case (GET_DESCRIPTOR_REQ):
                                        { // Needed to fix scoping 
                                                // Get Descriptor Request (usb_20.pdf, pg. 253)
                                                uint8_t wValueH = (setup.wValue >> 8);
                                                //uint8_t wValueL = (setup.wValue & 0xFF); 
                                                switch (wValueH)
                                                {
                                                        case (DESCRIPTOR_TYPE_DEVICE):
                                                                // Device Descriptor (usb_20.pdf, pg. 261)
                                                                // Send Device Descriptor from ProgMem 
                                                                sendProgMemPayload(&DeviceDescriptor, sizeof(USB_DeviceDescriptor_t), 255); 
                                                                break; 
                                                        case (DESCRIPTOR_TYPE_CONFIGURATION):
                                                                // Config Descriptor (usb_20.pdf, pg. 264)
                                                                // Send Configuration Descriptor from ProgMem 
                                                                sendProgMemPayload(&Configuration, sizeof(USB_Configuration_t), setup.wLength); 
                                                                break; 
                                                        default:
                                                                // Stall so that the host knows the request was unsupported. 
                                                                toStall = true;  
                                                                break; 
                                                }
                                        }
                                        break;
                                default: 
                                        break; 
                                case (SET_DESCRIPTOR_REQ):
                                        // Not supported, stall the ctl ep 
                                        toStall = true; 
                                        break;
                                case (GET_CONFIGURATION_REQ):
                                        // Return the proper configuration selector value (usb_20.pdf, pg. 253)
                                        // .bConfigurationValue in Config Descriptor is set to one -> send a one
                                        tx8(1); 
                                        break;
                                case (SET_CONFIGURATION_REQ):
                                                if (D40_RECIPIENT_DEVICE_MASK == (setup.bmRequestType & D40_RECIPIENT_MASK)) {
                                                        // Set a device configuration 
                                                        // only one available so initialize everything 
                                                        InitOtherEP(); 
                                                        setEP(EP_CTL_NUM); // keep EP0 selected
                                                        usbConfiguration = setup.wValue; //truncate the high byte and set the lower as the config 
                                                } else {
                                                        // not supported, stall the CTL Ep
                                                        toStall = true; 
                                                }
                                        break;
                                case (GET_INTERFACE_REQ):
                                        // arduino lib does nothing here 
                                        break;
                                case (SET_INTERFACE_REQ):
                                        // arduino lib does nothing here 
                                        break;
                        }
                        break;
                default:
                        // Class Interface Request //TODO: I think this might be vendor Check that 
                        //!: This implementation is currently trash
                        toStall = classInterfaceRequest(setup); 
                        // low byte of .wIndex in .bmRequestType specifies Interface index
                        break;
        }

        // Either stall or send the packet currently in the buffer 
        if (toStall) {
                stall(); 
        } else {
                UEINTX &= ~(1 << TXINI);
        } 

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

/* --------------------------- DATA_INITIALIZATION -------------------------- */
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

const SerialClass::USB_Configuration_t SerialClass::Configuration PROGMEM = 
{
        { // .ConfigurationDescriptor (usb_20.pdf, pg. 264)
                sizeof(SerialClass::USB_ConfigurationDescriptor_t), // .bLength: 9 byte struct 
                0x02, // .bDescriptorType -> Configuration Descriptor 
                sizeof(SerialClass::USB_Configuration_t), // .wTotalLength: Number of bytes that will be transmitted 
                2, // .bNumInterfaces: how many interfaces are active in this config // TODO: INCORRECT?
                1, // .bConfigurationValue: Number used to select this configuration 
                0, // .iConfiguration: Index Offset of String Descriptor for this config 
                ((1 << 7) | (0 << 6) | (1 << 5)), // .bmAttributes: self powered and remote wakeup enabled 
                (500 / 2) // .bMaxPower: Sets maximum power consumption to 500mA (2 mA increments)
        },
        { // .InterfaceAssociationDescriptor (https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-interface-association-descriptor)
                sizeof(SerialClass::USB_InterfaceAssociationDescriptor_t), // .bLength: 8 byte struct
                0x0B, // .bDescriptorType: 0x0B -> Interface Association Descriptor (IAD) 
                0, // .bFirstInterface: Interface index 0 is the first interface in this association 
                2, //.bInterfaceCount: 2 separate interfaces are in this association
                0x02, // .bFunctionClass: 0x02 Class Code
                0x02, // .bFunctionSubClass: 0x02 Subclass Code
                0, // .bFunctionProtocol: 0 Protocol Code 
                0 // .iFunction: function 0 
        },
        { // .ACMInterfaceDescriptor (usb_20.pdf, pg. 251)
                sizeof(SerialClass::USB_InterfaceDescriptor_t), // .bLength: 9 byte struct 
                4, // .bDescriptorType: 4 -> Interface Descriptor (usb_20.pdf, pg. 251)
                0, // .bInterfaceNumber: 0 -> first interface index 
                0, // .AlternateSetting: do not select an alternate setting 
                1, // .bNumEndpoints: 1 endpoint 
                0x02, // .bInterfaceClass: Base Class Communications and CDC Control (https://www.usb.org/defined-class-codes)
                0x02, // .bInterfaceSubClass: Sub Class CDC Control 
                0, // .bInterfaceProtocol: no class specific protocol 
                0 // .iInterface: first string descriptor describes this interface 
        },
        { // .HeaderFunctionalDescriptor (usbcdc11.pdf, pg. 45)
                sizeof(SerialClass::CDC_HeaderFunctionalDescriptor_t), // .bFunctionLength: 5 byte struct
                0x24, // .bDescriptorType: CS_Interface type 
                0x00, // .bDescriptorSubtype: 0x00 -> Header for Functional Descriptor 
                0x1001 // .bcdCDC: Version 1.10
        },
        { // .CallManagementFunctionalDescriptor (usbcdc11.pdf, pg. 45)
                sizeof(SerialClass::CDC_CallManagementFunctionalDescriptor_t), // .bFunctionLength: 5
                0x24, // .bDescriptorType: CS_Interface type
                0x01, // .bDescriptorSubtype: 0x01 -> Call Management Functioanl Descriptor
                0x01, // .bmCapabilities: D0 = 1 -> Device handles call management itself 
                1 // .bDataInterface: Interface index number of Data Class Interface used for call management 
        },
        { // .ACMControlManagementFunctionalDescriptor (usbcdc11.pdf, pg. 46)
                sizeof(SerialClass::CDC_ACMControlManagementFunctionalDescriptor_t), // .bFunctionLength: 4 bytes 
                0x24, // .bDescriptorType: CS_Interface type 
                0x02, // .bDescriptorSubtype: 0x02 -> Abstract Control Management Functional Descriptor
                0x06 // .bmCapabilities: D1 = 1 -> Device supports requests, D2 = 1 -> Device supports Send_Break 
        },
        { // .UnionFunctionalDescriptor (usbcdc11.pdf, pg. 51)
                sizeof(SerialClass::CDC_UnionFunctionalDescriptor_t), // .bFunctionLength: 5 byte struct 
                0x24, // .bDescriptorType: CS_Interface type 
                0x06, // .bDescriptorSubtype: 0x06 -> Union Functional Descriptor
                0, // .bMasterInterface: 0 -> interface 0 is the controlling interface
                1 // .bSlaveInterface0: 1 -> interface 1 is the slave interace 
        },
        { // .ACMEndpointDescriptor (usb_20.pdf, pg. 269)
                sizeof(SerialClass::USB_EndpointDescriptor_t), // .bLength: 7 byte struct 
                0x05, // .bDescriptorType: 0x05 -> Endpoint Descriptor
                0x81, // .bEndpointAddress: D7 = 1 -> IN endpoint, D0 = 1 -> Endpoint 1
                0x03, // .bmAttributes: 0x03 -> Interrupt Type Endpoint, Data Endpoint, No Sync 
                0x0010, // .wMaxPacketSize: HIGH(0x00) -> 1 transaction per microframe, LOW(0x10) -> 16 Byte Max Packet Size
                64 // .bInterval: 64 -> 64 milliseconds per polling interval (low/full speed = 1ms per .bInterval)
        },
        { // .DataInterfaceDescriptor (usb_20.pdf, pg. 267)
                sizeof(SerialClass::USB_InterfaceDescriptor_t), //.bLength 9 byte struct 
                4, // .bDescriptorType: 4 -> Interface Descriptor
                1, // .bInterfaceNumber: 1 -> second interface in this config 
                0, // .bAlternateSetting: do not select an alternate setting
                2, // .bNumEndpoints: This interfaces uses 2 endpoints 
                0x0A, // .bInterfaceClass: 0x0A -> CDC-Data Interface (https://www.usb.org/defined-class-codes)
                0, // .bInterfaceSubClass: N/A
                0, // .bInterfaceProtocol: 0 -> no class specific protocol
                0 // .biInterface: first string descriptor describes this interface
        },
        { // .OutEndpointDescriptor (usb_20.pdf, pg. 269)
                sizeof(SerialClass::USB_EndpointDescriptor_t), // .bLength 7 byte struct 
                5, // .bDescriptorType: 5 -> Endpoint Descriptor
                0x02, // .bEndpointAddress: D1 = 1 -> EP2
                0x02, // .bmAttributes: 0x02 -> Bulk Endpoint 
                64, // .wMaxPacketSize: 64 byte max packet size 
                0 // .bInterval: 0 -> Endpoint never NAK's 
        },
        { // .InEndpointDescriptor (usb_20.pdf, pg. 269)
                sizeof(SerialClass::USB_EndpointDescriptor_t), // .bLength 7 byte struct 
                5, // .bDescriptorType: 5 -> Endpoint Descriptor
                0x83, // .bEndpointAddress: D1, D0 = 1 -> EP3, D7 = 1 -> IN endpoint 
                0x02, // .bmAttributes: 0x02 -> Bulk Endpoint 
                64, // .wMaxPacketSize: 64 byte max packet size 
                0 // .bInterval: 0 -> Endpoint never NAK's 
        }
};
/* -------------------------------------------------------------------------- */

// Create Global Serial Object
//---------------------------
SerialClass Serial; 
//---------------------------


