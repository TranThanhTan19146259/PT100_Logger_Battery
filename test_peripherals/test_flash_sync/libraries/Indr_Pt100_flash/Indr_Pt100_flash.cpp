#include "Indr_Pt100_flash.h"

bool Indr_Pt100_flash::initFlash(uint8_t sck_pin, uint8_t miso_pin, uint8_t mosi_pin, uint8_t spi_clock)
{
    SPI.begin(sck_pin, miso_pin, mosi_pin, this->cs_pin);
    if (!this->begin()) {
        return 0;
    }
    this->setClock(spi_clock);
    return 1;
}

String Indr_Pt100_flash::getFlashBasicInfor()
{
    uint32_t jedec = this->getJEDECID();
    uint8_t manufacturer = (jedec >> 16) & 0xFF;
    uint8_t memType = (jedec >> 8) & 0xFF;
    uint8_t capacityCode = jedec & 0xFF;
    char buf_basic_infor[100];
    sprintf(buf_basic_infor, "JEDEC ID: 0x%06lX  (MFG=0x%02X TYPE=0x%02X CAP=0x%02X capacity: %lu bytes)", (unsigned long)jedec, manufacturer, memType, capacityCode, (unsigned long)this->getCapacity());
    String output;
    output = buf_basic_infor;
    return output;
    // Serial.printf("Reported capacity: %lu bytes\n", (unsigned long)this->getCapacity());
}

// bool Indr_Pt100_flash::checkEmptySector(uint32_t sector)
// {
//     if (this->readULong(sector * SECTOR_SIZE) == 0xFFFFFFFF) // empty
//     {
//         return true;
//     }
//     else return false;
// }

// bool Indr_Pt100_flash::writeU64Data(uint32_t addr, uint64_t U64Data)
// {
//     this->writeAnything(addr, U64Data);
//     return false;
// }

// bool Indr_Pt100_flash::readU64Data(uint32_t addr, uint64_t &U64Data)
// {
//     this->readAnything(addr, U64Data);
//     return false;
// }
