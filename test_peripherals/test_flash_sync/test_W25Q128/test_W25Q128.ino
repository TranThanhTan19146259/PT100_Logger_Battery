/*
  ESP32 External SPI Flash Demo
  esp32-ext-flash-demo.ino
  Demonstrates use of W25Q64 Flash memory module
  Uses ESP32-S3 DevKit1
  Uses SPIMemory Library

  1 - Read Flash ID
  2 - Read Flash Capacity
  3 - Erase 4kB sector
  4 - Write and Read
  
  DroneBot Workshop 2025
  https://dronebotworkshop.com
*/

// Include Required Libraries
#include <SPI.h>
#include <SPIMemory.h>
#include <inttypes.h>

// --- Your wiring on ESP32-S3 DevKitC-1 ---
static const int PIN_SCK = 18;   // CLK
static const int PIN_MISO = 19;  // DO  -> MISO 
static const int PIN_MOSI = 23;  // DI  -> MOSI
static const int PIN_CS = 22;    // CS

SPIFlash flash(PIN_CS);

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== W25Q128 Quick Test ===");

    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);

    if (!flash.begin()) {
        Serial.println("Flash NOT detected. Check 3V3, GND, CS, and wiring.");
        while (1) delay(10);
    }
    flash.setClock(128000);
    // NEW: getJEDECID() returns a 32-bit value like 0xEF4017 for W25Q64
    uint32_t jedec = flash.getJEDECID();
    uint8_t manufacturer = (jedec >> 16) & 0xFF;
    uint8_t memType = (jedec >> 8) & 0xFF;
    uint8_t capacityCode = jedec & 0xFF;

    Serial.printf("JEDEC ID: 0x%06lX  (MFG=0x%02X TYPE=0x%02X CAP=0x%02X)\n",
                    (unsigned long)jedec, manufacturer, memType, capacityCode);
    Serial.printf("Reported capacity: %lu bytes\n", (unsigned long)flash.getCapacity());

    const uint32_t addr = 0x00000;  // sector 0
    Serial.println("Erasing 4KB sector @ 0x000000...");
    // if (!flash.eraseSector(addr)) Serial.println("Erase FAILED");
    // while(flash.busy());
    delay(100);
    // Now try writing
    // if (flash.writeByte(0x0000, 0xAB)) {
    // Serial.println("Write: OK");
    // } else {
    // Serial.println("Write: FAIL");
    // }
    // NEW: make it non-const for writeCharArray (or use writeAnything)
    uint64_t U64_data_W = 0xAABBCCDDEEFF0011;
    uint32_t U64_data_R[2];

    ESP_LOGD(TAG, "FIRST DATA");
    bool okW = flash.writeAnything(addr, U64_data_W);  // includes '\0'
    bool okR = flash.readAnything(addr, U64_data_R[0]);
    bool okR1 = flash.readAnything(addr+4, U64_data_R[1]);
    // Serial.printf("Write: %s\n", okW ? "OK" : "FAIL");
    Serial.printf("Read: %s\n", okR ? "OK" : "FAIL");
    Serial.printf("Read1: %s\n", okR1 ? "OK" : "FAIL");
    ESP_LOGD(TAG, "addr: %x 0x%x", addr,U64_data_R[0]);
    ESP_LOGD(TAG, "addr: %x 0x%x", addr + 4,U64_data_R[1]);
    // ESP_LOGD(TAG, "addr: %x 0x%016" PRIx64, addr,U64_data_R[0]);
    // ESP_LOGD(TAG, "addr: %x 0x%016" PRIx64, addr+4,U64_data_R[1]);
    // delay(5000);
    // ESP_LOGD(TAG, "SECOND DATA");
    // flash.writeAnything(addr + 8, U64_data_W + 1);
    // flash.readAnything(addr + 8, U64_data_R);
    // Serial.printf("Write: %s\n", okW ? "OK" : "FAIL");
    // Serial.printf("Read: %s\n", okR ? "OK" : "FAIL");
    // ESP_LOGD(TAG, "addr: %x 0x%016" PRIx64, addr + 8,U64_data_R);
    //   bool okR = flash.readCharArray(addr, buf, sizeof(buf));
}

void loop() {}