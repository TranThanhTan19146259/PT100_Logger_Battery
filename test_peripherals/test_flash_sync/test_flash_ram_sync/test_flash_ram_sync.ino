// erase 4 sectors (16KB)
// 940 data points
#include <ArduinoJson.h>
#include <time.h>
#include <inttypes.h>

#include <SPI.h>
#include <SPIMemory.h>

// init flash
// write flash (addr, data)
// | check sector first address
// | check empty
// | | check EEPROM data bits (if first address of every sector return 1 => it means that sector is not empty else return 0)
// | check clear flash if not empty

// read flash (addr, data)
// check sector busy (in order to clear and write data)
// convertUnixTimeToHumanDate and back


#define MQTT_SIZE_RAM 128
#define FLASH_SIZE_RAM 512
uint64_t sector0[FLASH_SIZE_RAM] = {}; // 4KB

uint64_t sector0Read[FLASH_SIZE_RAM] = {};
// --- Your wiring on ESP32-S3 DevKitC-1 ---
static const int PIN_SCK = 18;   // CLK
static const int PIN_MISO = 19;  // DO  -> MISO
static const int PIN_MOSI = 23;  // DI  -> MOSI
static const int PIN_CS = 22;    // CS

SPIFlash flash(PIN_CS);

float temp_data = 90.90;
String time_data = "10/10/2025 10:10:10";

template <typename T>
union convert_data{
  uint8_t unmarshall[sizeof(T)];
  T marshall;
};

uint32_t convertHumanDateToUnixTime(String dateTime)
{
  struct tm t = {0};
    
    // Parse the date string (MM/DD/YYYY HH:MM:SS)
    sscanf(dateTime.c_str(), "%d/%d/%d %d:%d:%d",
           &t.tm_mon, &t.tm_mday, &t.tm_year,
           &t.tm_hour, &t.tm_min, &t.tm_sec);
    
    // Adjust values for struct tm
    t.tm_mon -= 1;        // Month is 0-11
    t.tm_year -= 1900;    // Year since 1900
    t.tm_isdst = -1;      // Let mktime determine DST
    
    // Convert to Unix timestamp
    time_t timestamp = mktime(&t);
    return timestamp;
}

void convertUnixTimeToHumanDate(uint32_t unixTime, struct tm *output_tm)
{
    time_t rawtime = unixTime;
    struct tm *timeinfo = localtime(&rawtime);
    
    // Copy the time data to the output pointer
    if (output_tm != NULL && timeinfo != NULL) {
        *output_tm = *timeinfo;
    }
}

void getInitInfor_W25Q128()
{
  uint32_t jedec = flash.getJEDECID();
  uint8_t manufacturer = (jedec >> 16) & 0xFF;
  uint8_t memType = (jedec >> 8) & 0xFF;
  uint8_t capacityCode = jedec & 0xFF;

  Serial.printf("JEDEC ID: 0x%06lX  (MFG=0x%02X TYPE=0x%02X CAP=0x%02X)\n",
                  (unsigned long)jedec, manufacturer, memType, capacityCode);
  Serial.printf("Reported capacity: %lu bytes\n", (unsigned long)flash.getCapacity());

  // const uint32_t addr = 0x00000;  // sector 0
  // Serial.println("Erasing 4KB sector @ 0x000000...");
    // while (flash.eraseSector(addr));
}

void setup()
{
  Serial.begin(115200);
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);

  if (!flash.begin()) {
      Serial.println("Flash NOT detected. Check 3V3, GND, CS, and wiring.");
      while (1) delay(10);
  }
  flash.setClock(128000);
  
  getInitInfor_W25Q128();

  const uint32_t addr = 0x00000;  // sector 0
  Serial.println("-------START-----");

  bool write_check = flash.writeAnything(addr,sector0[0]);
  Serial.printf("Write: %s\n", write_check ? "OK" : "FAIL");
  flash.readAnything(addr, sector0Read[0]);
  ESP_LOGD(TAG, "tempTimeData = 0x%016" PRIx64, sector0Read[0]);

  convert_data<uint64_t> timeTempDataFromFlash; 
  timeTempDataFromFlash.marshall = sector0Read[0];
  convert_data<uint32_t>unixTimeStamp;
  unixTimeStamp.unmarshall[0] = timeTempDataFromFlash.unmarshall[0];
  unixTimeStamp.unmarshall[1] = timeTempDataFromFlash.unmarshall[1];
  unixTimeStamp.unmarshall[2] = timeTempDataFromFlash.unmarshall[2];
  unixTimeStamp.unmarshall[3] = timeTempDataFromFlash.unmarshall[3];
  convert_data<float>tempData;
  tempData.unmarshall[0] = timeTempDataFromFlash.unmarshall[4];
  tempData.unmarshall[1] = timeTempDataFromFlash.unmarshall[5];
  tempData.unmarshall[2] = timeTempDataFromFlash.unmarshall[6];
  tempData.unmarshall[3] = timeTempDataFromFlash.unmarshall[7];
  struct tm timeInfo;
  char time_buf[30];
  convertUnixTimeToHumanDate(unixTimeStamp.marshall, &timeInfo);
  ESP_LOGD(TAG,"temp: %.2f time: %s", tempData.marshall, asctime(&timeInfo));
  

}

void loop()
{

}