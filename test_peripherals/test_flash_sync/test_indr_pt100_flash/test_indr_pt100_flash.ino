#include "Indr_Pt100_flash.h"

uint8_t PIN_SCK = 18;   // CLK
uint8_t PIN_MISO = 19;  // DO  -> MISO
uint8_t PIN_MOSI = 23;  // DI  -> MOSI
uint8_t PIN_CS = 22;    // CS
uint32_t spi_speed_clock = 128000; //kHz

Indr_Pt100_flash pt100Flash(PIN_CS);

// init flash
// write flash (addr, data)
// | check sector first address
// | check empty
// | | check EEPROM data bits (if first address of every sector return 1 => it means that sector is not empty else return 0)
// | check clear flash if not empty

// read flash (addr, data)
// check sector busy (in order to clear and write data)
// convertUnixTimeToHumanDate and back

void setup()
{
    Serial.begin(115200);
    if (!pt100Flash.initFlash(PIN_SCK, PIN_MISO, PIN_MOSI, spi_speed_clock))
    {
        Serial.println("Flash NOT detected. Check 3V3, GND, CS, and wiring.");
    }
    
    Serial.println(pt100Flash.getFlashBasicInfor());
    uint64_t tempTimeDataRaw;
    // pt100Flash.writeAnything(0, )
    pt100Flash.readAnything(0, tempTimeDataRaw);
    ESP_LOGD(TAG, "tempTimeData = 0x%016" PRIx64, tempTimeDataRaw);
    // ESP_LOGD(TAG, "timeTempRaw: ")

}

void loop()
{

}