// Date: 10/18/24
// Desc: Implementation file for the storage class

#include "storage.h"
#include <stdint.h>
#include "board.h"
#include "gpio.h"
#include <Adafruit_FRAM_SPI.h>

/* ---------------------- STORAGE_CLASS_IMPLEMENTATION ---------------------- */
Storage::Storage()
{
        return; 
}

uint8_t Storage::init()
{
        // FRAM !(HOLD) is not used by the library, set HIGH to disable
        gpioInit(PIN::FRAM_HOLD, PIN_MODE::OUTPUT_M, PIN_STATE::HIGH_S);

        // Instantiate the FRAM object
        fram = Adafruit_FRAM_SPI(PIN::FRAM_CS); 

        // Verify that fram is connected 
        if (!fram.begin()){
                return 1; // ERROR, no connection 
        }

        // This FRAM uses 3byte addressing, tell the library
        fram.setAddressSize(3); 

        // Enable writing, (this can stay enabled)
        fram.writeEnable(true); 

        // Check the contents of the FRAM to determine status 
        if (fram.read8(0) != 0) {
                state = STORAGE_STATE::EMPTY; 
        }
        return 0; 
}

uint8_t Storage::eraseAll()
{
        // Write 0 to [0, FRAM_WORD_SIZE-1] addresses
        for(uint32_t addr = 0; addr <= (FRAM_WORD_SIZE - 1); addr++)
        {
                fram.write8(addr, 0);
        }
        // Read each address to make sure they are all 0
        for(uint32_t addr = 0; addr <= (FRAM_WORD_SIZE - 1); addr++)
        {
            if (fram.read8(addr) != 0) //execute if address value is not 0
                {
                return 1; // 1: error
                }
        }
        return 0; // 0: success 
}

Storage::STORAGE_STATE Storage::getState()
{
        return state;
}

// Instantiate the global storage class
Storage storage; 
/* -------------------------------------------------------------------------- */