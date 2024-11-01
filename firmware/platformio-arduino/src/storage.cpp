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
        if (fram.read8(0) == 0) {
                // fram is empty, make sure that it is 
                eraseAll(); 
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
        state = STORAGE_STATE::EMPTY; 
        currentFramAddr = 0; 
        return 0; // 0: success 
}

Storage::STORAGE_STATE Storage::getState()
{
        return state;
}

uint8_t Storage::writeAccel(const uint32_t time, const uint16_t xAccel, 
                                const uint16_t yAccel, const uint16_t zAccel)
{
        AccelPkt pkt = {
                        (NORMAL | ACCEL_ONLY),
                        time,
                        xAccel,
                        yAccel, 
                        zAccel
                        };
        
        return writePkt(&pkt, sizeof(pkt));
}

uint8_t Storage::writeGyro(const uint32_t time, const uint16_t xRot, 
                                const uint16_t yRot, const uint16_t zRot)
{
        GyroPkt pkt = {
                        (NORMAL | GYRO_ONLY),
                        time,
                        xRot,
                        yRot,
                        zRot
                        };

        return writePkt(&pkt, sizeof(pkt));
}

uint8_t Storage::writePressure(const uint32_t time, const uint32_t pressure)
{
        PressurePkt pkt = {
                        (NORMAL | PRESSURE_ONLY),
                        time,
                        pressure
                        };

        return writePkt(&pkt, sizeof(pkt));
}

uint8_t Storage::writeAccelGyro(const uint32_t time, const uint16_t xAccel, 
                                const uint16_t yAccel, const uint16_t zAccel,
                                const uint16_t xRot, const uint16_t yRot, 
                                const uint16_t zRot)
{
        AccelGyroPkt pkt = {
                        (NORMAL | ACCEL_AND_GYRO),
                        time,
                        xAccel,
                        yAccel, 
                        zAccel,
                        xRot,
                        yRot,
                        zRot
                        };

        return writePkt(&pkt, sizeof(pkt));
}

uint8_t Storage::writeAccelGyroPressure(const uint32_t time, const uint16_t xAccel, 
                                const uint16_t yAccel, const uint16_t zAccel,
                                const uint16_t xRot, const uint16_t yRot, 
                                const uint16_t zRot, const uint32_t pressure)
{
        AccelGyroPressurePkt pkt = {
                        (NORMAL | ACCEL_AND_GYRO),
                        time,
                        xAccel,
                        yAccel, 
                        zAccel,
                        xRot,
                        yRot,
                        zRot,
                        pressure
                        };

        return writePkt(&pkt, sizeof(pkt));
}

uint8_t Storage::printFRAM()
{
        uint32_t timeoutStartMS = millis(); 
        while (!Serial)
        {
                // wait for the USB CDC Serial port to be initialized
                if ((millis() - timeoutStartMS) > 2000) { // 2s timeout
                        return 1; // error 
                }
                delay(1); 
        }

        // print all bytes in FRAM 
        for (uint32_t i = 0; i < currentFramAddr; i++)
        {
                Serial.write(fram.read8(i)); 
        }
        return 0; 
}


uint8_t Storage::computeParity(void * const data, const uint8_t size)
{
        uint8_t * check_data = reinterpret_cast<uint8_t*>(data);
        uint8_t parity = *check_data;
        for (uint8_t i = 1; i < size; i++)
        {
                parity ^= *(check_data + i); 
        }
        parity ^= (parity >> 4); 
        parity ^= (parity >> 2); 
        parity ^= (parity >> 1); 
        return (parity & 0x01);
}

uint8_t Storage::writePkt(void * const data, const uint8_t size)
{
        if (isParityEnabled) {
                *(reinterpret_cast<uint8_t *>(data)) |= computeParity(data, size); 
                }

        uint8_t status = 1; 
        if ((currentFramAddr + size) < FRAM_WORD_SIZE) {
                status = !(fram.write(currentFramAddr, reinterpret_cast<uint8_t *>(data), size));
                currentFramAddr += size * (!status); 
        }
        return status; 
}

// Instantiate the global storage class
Storage storage; 
/* -------------------------------------------------------------------------- */