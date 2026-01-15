#include "sync_data_flash_to_sv.h"

static const int PIN_SCK = 18;   // CLK
static const int PIN_MISO = 19;  // DO  -> MISO
static const int PIN_MOSI = 23;  // DI  -> MOSI
static const int PIN_CS = 22;    // CS

SPIFlash flash(PIN_CS);



// template <typename T>
// union convert_data{
//   uint8_t unmarshall[sizeof(T)];
//   T marshall;
// };

uint32_t convertHumanDateToUnixTime(String dateTime)
{
  struct tm t = {0};
    
    // Parse the date string (MM/DD/YYYY HH:MM:SS)
    sscanf(dateTime.c_str(), "%d/%d/%d %d:%d:%d",
           &t.tm_mday, &t.tm_mon, &t.tm_year,
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
}

void save_data_offline_to_flash()
{
    static uint32_t t_save_offline_data;
    if (millis() - t_save_offline_data > myRam.pt100_data.sampleRate * 1000)
    {
        uint64_t U64_data_R;
        flash.readAnything(myRam.flashData_sync.currentFlashAddr, U64_data_R);
        // ESP_LOGD(TAG, "u64Data = 0x%016" PRIx64, U64_data_R);

        if (U64_data_R == 0xFFFFFFFFFFFFFFFF) // empty address
        {
            convert_data<float> tempData;
            tempData.marshall = myRam.pt100_data.temp;
            uint32_t unixTimeStamp;
            unixTimeStamp = convertHumanDateToUnixTime(myRam.ntp_time.ntpDateTimeString);
            ESP_LOGD(TAG, "unix time: %d", unixTimeStamp);
            convert_data<uint32_t> timeData;
            timeData.marshall = unixTimeStamp;
            convert_data<uint64_t> timeTempData;
            timeTempData.unmarshall[0] = timeData.unmarshall[0]; 
            timeTempData.unmarshall[1] = timeData.unmarshall[1];
            timeTempData.unmarshall[2] = timeData.unmarshall[2];
            timeTempData.unmarshall[3] = timeData.unmarshall[3];

            timeTempData.unmarshall[4] = tempData.unmarshall[0];
            timeTempData.unmarshall[5] = tempData.unmarshall[1];
            timeTempData.unmarshall[6] = tempData.unmarshall[2];
            timeTempData.unmarshall[7] = tempData.unmarshall[3];
            // flash.readAnything(myRam.flashData_sync.currentFlashAddr, U64_data_R);
            ESP_LOGD(TAG, "data W = 0x%016" PRIx64, timeTempData.marshall);
            
            if (!flash.writeAnything(myRam.flashData_sync.currentFlashAddr, timeTempData.marshall))
            {
                ESP_LOGD(TAG, "data is written failed !");
            }
            else
            {
                uint64_t timeTempDataRaw;

                flash.readAnything(myRam.flashData_sync.currentFlashAddr, timeTempDataRaw);
                ESP_LOGD(TAG, "Data R = 0x%016" PRIx64, timeTempDataRaw);

            }
            //  flash.writeAnything(myRam.flashData_sync.currentFlashAddr, timeTempData.marshall);
            ESP_LOGD(TAG, "addr stored: %x", myRam.flashData_sync.currentFlashAddr);
            myRam.flashData_sync.currentFlashAddr += 8;
        }
        else
        {
            ESP_LOGD(TAG, "sector %d is not empty", (myRam.flashData_sync.currentFlashAddr / SECTOR_SIZE));
            if (!flash.eraseSector(myRam.flashData_sync.currentFlashAddr)) Serial.println("Erase FAILED");
            // flash.eraseSector(myRam.flashData_sync.currentFlashAddr);
            // delay(100);
        }
        //  myRam.flashData_sync.currentFlashAddr
        t_save_offline_data = millis();
    }
    
}

void read_pt100_flash(uint32_t addr, uint64_t &U64Data)
{
    uint64_t u64Data;
    flash.readAnything(addr, u64Data);
    U64Data = u64Data;
    // ESP_LOGD(TAG, "Addr %d Insde RF Data R = 0x%016" PRIx64, addr ,U64Data);
}


void init_sync_flashData()
{
    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);
    if (!flash.begin()) {
      Serial.println("Flash NOT detected. Check 3V3, GND, CS, and wiring.");
    //   while (1) delay(10);
    }
    flash.setClock(128000);
    getInitInfor_W25Q128();

    myRam.flashData_sync.sync_state = FIRST_STARTUP_FLASH_MEMORY;
}

void handle_sync_flashData()
{
    static uint32_t t_sync_flash_data;
    switch (myRam.flashData_sync.sync_state)
    {
    case START_ASSIGN_DATA_TO_FLASH:
        {
            save_data_offline_to_flash();
            if (myRam.mqtt_config_data.isConnectToBroker == 1 && myRam.flashData_sync.currentFlashAddr != 0) // there is data remains inside flash memory
            {
                myRam.flashData_sync.total_offline_data_stored = (myRam.flashData_sync.currentFlashAddr) / 8;
                myRam.flashData_sync.total_sector_used = (myRam.flashData_sync.currentFlashAddr) / SECTOR_SIZE;
                ESP_LOGD(TAG, "total data stored: %d", myRam.flashData_sync.total_offline_data_stored);
                myRam.flashData_sync.sync_state = START_SEND_FLASH_DATA_TO_SV;
                // myRam.flashData_sync.currentFlashAddr = 0;
            }
            
            break;
        }
    case START_SYNC_FLASH_DATA_TO_SV:
        {
            t_sync_flash_data = millis();
            myRam.flashData_sync.sync_state = WAIT_TO_SEND_FLASH_DATA_TO_SV;
            break;
        }
    case WAIT_TO_SEND_FLASH_DATA_TO_SV:
        {
            if (millis() - t_sync_flash_data > 200)
            {
                myRam.flashData_sync.sync_state = START_SEND_FLASH_DATA_TO_SV;
                t_sync_flash_data = millis();
            }
            
            break;
        }
    case CLEAR_FLASH_DATA:
        {
            for (uint32_t i = 0; i < myRam.flashData_sync.total_sector_used; i++)
            {
                flash.eraseSector(i * SECTOR_SIZE);
            }
            myRam.flashData_sync.ptr_buf_flash_offline_data = 0;
            myRam.flashData_sync.currentFlashAddr = 0;
            myRam.flashData_sync.sync_state = DONE_SYNC_FLASH_DATA_TO_SV;

            break;
        }
    default:
        break;
    }
}
