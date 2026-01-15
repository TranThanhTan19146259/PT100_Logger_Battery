#pragma once

#include <SPI.h>
#include <SPIMemory.h>
#include <inttypes.h>

#define SECTOR_SIZE 4096 // 4KB

class Indr_Pt100_flash : public SPIFlash
{
private:
    /* data */
    uint8_t sck_pin;
    uint8_t miso_pin;
    uint8_t mosi_pin;
    uint32_t spi_clock;
    uint8_t cs_pin;
public:
    // SCK, MISO, MOSI, CS, CLOCK_SPEED
    Indr_Pt100_flash(uint8_t cs_pin) : SPIFlash(cs_pin){}
    bool initFlash(uint8_t sck_pin, uint8_t miso_pin, uint8_t mosi_pin, uint8_t spi_clock);
    String getFlashBasicInfor();
    // bool checkEmptySector(uint32_t sector);
    bool writeU64Data(uint32_t addr, uint64_t U64Data);
    bool readU64Data(uint32_t addr, uint64_t &U64Data);
};