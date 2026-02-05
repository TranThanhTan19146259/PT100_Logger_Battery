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
        timeinfo->tm_year += 1900;
        timeinfo->tm_mon += 1;
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

// void save_data_offline_to_flash_with_time(String timeSnapShot)
// {
//     ESP_LOGD(TAG, "rawTimeSnapShot before: %s", timeSnapShot.c_str());
// }

void save_data_offline_to_flash(String dateTimeString)
{
        ESP_LOGD(TAG, "rawTime before: %s", dateTimeString.c_str());
        if (myRam.flashData_sync.flash_init_ok == 1)
        {
            uint64_t U64_data_R = 0xFFFFFFFFFFFFFFFF;
            // flash.readAnything(myRam.flashData_sync.flashAddrTail, U64_data_R);
            // ESP_LOGD(TAG, "u64Data = 0x%016" PRIx64, U64_data_R);

            if (U64_data_R == 0xFFFFFFFFFFFFFFFF) // empty address
            {
                convert_data<float> tempData;
                tempData.marshall = myRam.pt100_data.temp;
                uint32_t unixTimeStamp;
                // unixTimeStamp = convertHumanDateToUnixTime(myRam.ntp_time.ntpDateTimeString);
                unixTimeStamp = convertHumanDateToUnixTime(dateTimeString);
                ESP_LOGD(TAG, "unix time: %d rawTime after: %s", unixTimeStamp, dateTimeString.c_str());
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
                // flash.readAnything(myRam.flashData_sync.flashAddrTail, U64_data_R);
                // ESP_LOGD(TAG, "data W = 0x%016" PRIx64, timeTempData.marshall);
                
                if (!flash.writeAnything(myRam.flashData_sync.flashAddrTail, timeTempData.marshall))
                {
                    ESP_LOGD(TAG, "data is written failed !");
                }
                else
                {
                    uint64_t timeTempDataRaw;

                    flash.readAnything(myRam.flashData_sync.flashAddrTail, timeTempDataRaw);
                    ESP_LOGD(TAG, "Data R = 0x%016" PRIx64, timeTempDataRaw);

                }
                myRam.flashData_sync.flashAddrTail += 8;
                // myRam.flashData_sync.flash_save_data_tick = 0;
                //  flash.writeAnything(myRam.flashData_sync.flashAddrTail, timeTempData.marshall);
                // ESP_LOGD(TAG, "addr stored: %x", myRam.flashData_sync.flashAddrTail);
            }
            else
            {
                // ESP_LOGD(TAG, "sector %d is not empty", (myRam.flashData_sync.flashAddrTail / SECTOR_SIZE));
                // if (!flash.eraseSector(myRam.flashData_sync.flashAddrTail)) Serial.println("Erase FAILED");
                // flash.eraseSector(myRam.flashData_sync.flashAddrTail);
                // delay(100);
            }
            myRam.flashData_sync.total_offline_data_stored = (myRam.flashData_sync.flashAddrTail - myRam.flashData_sync.flashAddrHead) / 8;
            // myRam.flashData_sync.flashAddrTail += 8;
            //  myRam.flashData_sync.flashAddrTail
            // myRam.flashData_sync.flashAddrTail += 8;
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
    uint8_t count_init_flash = 0;
    if (!flash.begin()) {
        Serial.println("Flash NOT detected. Check 3V3, GND, CS, and wiring.");
        myRam.flashData_sync.flash_init_ok = 0;
        count_init_flash = 0;
    }
    else
    {
        count_init_flash = 1;
    }
    flash.setClock(128000);
    getInitInfor_W25Q128();

    myRam.flashData_sync.sync_state = FIRST_STARTUP_FLASH_MEMORY;
    convert_data<uint32_t> headFlashAddr;
    convert_data<uint32_t> tailFlashAddr;
    for (uint8_t i = 0; i < 4; i++)
    {
        headFlashAddr.unmarshall[i] = EEPROM.read(EEPROM_HEAD_FLASH_ADDR_OFFSET+i); 
        tailFlashAddr.unmarshall[i] = EEPROM.read(EEPROM_TAIL_FLASH_ADDR_OFFSET+i); 
    }
    myRam.flashData_sync.flashAddrHead = headFlashAddr.marshall;     
    myRam.flashData_sync.flashAddrTail = tailFlashAddr.marshall;
    
    myRam.flashData_sync.flashAddrHead = 0;
    myRam.flashData_sync.flashAddrTail = 0;
    if (myRam.flashData_sync.flashAddrHead > myRam.flashData_sync.flashAddrTail)
    {
        flash.eraseChip();
        myRam.flashData_sync.flashAddrTail = 0;
        myRam.flashData_sync.flashAddrHead = 0;
        myRam.flashData_sync.sync_state = CLEAR_FLASH_DATA;
    }
    else
    {
        myRam.flashData_sync.total_offline_data_stored = (myRam.flashData_sync.flashAddrTail - myRam.flashData_sync.flashAddrHead) / 8;
    }
    if (myRam.flashData_sync.total_offline_data_stored == 0)
    {
        flash.eraseChip();
        myRam.flashData_sync.flashAddrTail = 0;
        myRam.flashData_sync.flashAddrHead = 0;
        myRam.flashData_sync.sync_state = CLEAR_FLASH_DATA;
    }
    if (count_init_flash != 0)
    {
        while (1)
        {
            uint64_t U64_data_R;
            flash.readAnything(myRam.flashData_sync.flashAddrTail, U64_data_R);
            // ESP_LOGD(TAG, "u64Data = 0x%016" PRIx64, U64_data_R);
            if (U64_data_R == 0xFFFFFFFFFFFFFFFF)
            {
                ESP_LOGD(TAG, "sector %d is empty addr: %d", (myRam.flashData_sync.flashAddrTail / SECTOR_SIZE), myRam.flashData_sync.flashAddrTail);
                myRam.flashData_sync.flash_init_ok = 1;
                break;
            }
            else
            {
                myRam.flashData_sync.flashAddrTail += 8;
            }
            // delay(1000);
        }
    }
    ESP_LOGD(TAG, "init headAddr: %d tailAddr: %d flash_init: %d", myRam.flashData_sync.flashAddrHead, myRam.flashData_sync.flashAddrTail, myRam.flashData_sync.flash_init_ok);    
    
    // pinMode(27, INPUT_PULLUP);
}

void handle_flash_addresses()
{
    static uint32_t tailFlashAddr_temp = 0;
    if (tailFlashAddr_temp != myRam.flashData_sync.flashAddrTail)
    {
        tailFlashAddr_temp = myRam.flashData_sync.flashAddrTail;
        convert_data<uint32_t> tailFlashAddr_converted;
        tailFlashAddr_converted.marshall = tailFlashAddr_temp;
        for (uint8_t i = 0; i < 4; i++)
        {
            EEPROM.write(EEPROM_TAIL_FLASH_ADDR_OFFSET + i, tailFlashAddr_converted.unmarshall[i]); 
            EEPROM.commit();
        }
        ESP_LOGD(TAG, "write tail addr: %d", tailFlashAddr_temp);
    }
    static uint32_t headFlashAddr_temp = 0;
    if (headFlashAddr_temp != myRam.flashData_sync.flashAddrHead)
    {
        headFlashAddr_temp = myRam.flashData_sync.flashAddrHead;
        convert_data<uint32_t> headFlashAddr_converted;
        headFlashAddr_converted.marshall = headFlashAddr_temp;
        for (uint8_t i = 0; i < 4; i++)
        {
            EEPROM.write(EEPROM_HEAD_FLASH_ADDR_OFFSET + i, headFlashAddr_converted.unmarshall[i]); 
            EEPROM.commit();
        }
        // ESP_LOGD(TAG, "write head addr: %d", headFlashAddr_temp);
    }
    
}

void handle_sync_flashData()
{
    handle_flash_addresses();
    // if (digitalRead(27) == 0)
    // {
    //     flash.eraseChip();
    //     myRam.flashData_sync.sync_state = CLEAR_FLASH_DATA;
    //     ESP_LOGD(TAG, "clear flash manually");
    // }
    
    static uint32_t t_sync_flash_data;
    static uint32_t t_wait_for_clearing_flash_data;
    switch (myRam.flashData_sync.sync_state)
    {
    case START_ASSIGN_DATA_TO_FLASH:
        {
            // static uint32_t t_save_offline_data;
            // if (millis() - t_save_offline_data > myRam.pt100_data.sampleRate * 1000)
            
                // ESP_LOGD(TAG, "-------------SAVE FLASH DATA-----------------");
                // save_data_offline_to_flash();
                // myRam.pt100_data.timer_tick = 0;
            //     myRam.flashData_sync.flash_save_data_tick = 0;
            //     // t_save_offline_data = millis();
            // }
            // if (myRam.flashData_sync.flashAddrTail == 0)
            // {
            //     flash.eraseChip();
            // }
            // uint64_t U64_data_R;
            // flash.readAnything(myRam.flashData_sync.flashAddrTail, U64_data_R);
            // ESP_LOGD(TAG, "u64Data = 0x%016" PRIx64, U64_data_R);
            
            if (myRam.mqtt_config_data.isConnectToBroker == 1 && myRam.flashData_sync.flashAddrTail != 0) // there is data remains inside flash memory
            {
                // myRam.flashData_sync.total_sector_used = ((myRam.flashData_sync.flashAddrTail) / SECTOR_SIZE) + 1;
                myRam.flashData_sync.sync_state = START_SEND_FLASH_DATA_TO_SV;
                // myRam.flashData_sync.flashAddrTail = 0;
            }
            myRam.flashData_sync.total_offline_data_stored = (myRam.flashData_sync.flashAddrTail - myRam.flashData_sync.flashAddrHead) / 8;
            break;
        }
    case START_SYNC_FLASH_DATA_TO_SV:
        {
            t_sync_flash_data = millis();
            myRam.flashData_sync.sync_state = WAIT_TO_SEND_FLASH_DATA_TO_SV;
            myRam.flashData_sync.total_offline_data_stored = (myRam.flashData_sync.flashAddrTail - myRam.flashData_sync.flashAddrHead) / 8;
            break;
        }
    case WAIT_TO_SEND_FLASH_DATA_TO_SV:
        {
            if (millis() - t_sync_flash_data > 2000)
            {
                myRam.flashData_sync.sync_state = START_SEND_FLASH_DATA_TO_SV;
                t_sync_flash_data = millis();
            }
            
            break;
        }
    case CLEAR_FLASH_DATA:
        {
            myRam.flashData_sync.total_sector_used = ((myRam.flashData_sync.flashAddrTail) / SECTOR_SIZE) + 1;
            ESP_LOGD(TAG, "total sector used: %d", myRam.flashData_sync.total_sector_used);
            for (uint32_t i = 0; i < myRam.flashData_sync.total_sector_used; i++)
            {
                flash.eraseSector(i * SECTOR_SIZE);
            }
            // delay(5000);
            
            // flash.eraseChip();

            myRam.flashData_sync.ptr_buf_flash_offline_data = 0;
            myRam.flashData_sync.flashAddrTail = 0;
            myRam.flashData_sync.flashAddrHead = 0;
            myRam.flashData_sync.total_offline_data_stored = (myRam.flashData_sync.flashAddrTail - myRam.flashData_sync.flashAddrHead) / 8;
            for (uint8_t i = 0; i < 4; i++)
            {
                EEPROM.write(EEPROM_HEAD_FLASH_ADDR_OFFSET + i, 0); 
                EEPROM.commit();
                // EEPROM.write(EEPROM_TAIL_FLASH_ADDR_OFFSET + i, 0); 
                // EEPROM.commit();
            }

            for (uint8_t i = 0; i < 4; i++)
            {
                // EEPROM.write(EEPROM_HEAD_FLASH_ADDR_OFFSET + i, 0); 
                // EEPROM.commit();
                EEPROM.write(EEPROM_TAIL_FLASH_ADDR_OFFSET + i, 0); 
                EEPROM.commit();
            }
            convert_data<uint32_t> headFlashAddr;
            convert_data<uint32_t> tailFlashAddr;
            for (uint8_t i = 0; i < 4; i++)
            {
                headFlashAddr.unmarshall[i] = EEPROM.read(EEPROM_HEAD_FLASH_ADDR_OFFSET+i); 
                tailFlashAddr.unmarshall[i] = EEPROM.read(EEPROM_TAIL_FLASH_ADDR_OFFSET+i); 
            }
            myRam.flashData_sync.flashAddrHead = headFlashAddr.marshall;     
            myRam.flashData_sync.flashAddrTail = tailFlashAddr.marshall;
            ESP_LOGD(TAG, "headAddr: %d tailAddr: %d", myRam.flashData_sync.flashAddrHead, myRam.flashData_sync.flashAddrTail);
            t_wait_for_clearing_flash_data = millis();
            myRam.flashData_sync.sync_state = WAIT_FOR_CLEARING_FLASH_DATA;
            // while (1)
            // {
            //     ESP_LOGD(TAG, "CLEAR FLASHHHHHH");
            // }
            
            break;
        }
        case WAIT_FOR_CLEARING_FLASH_DATA:
        {
            if (millis() - t_wait_for_clearing_flash_data > 1000)
            {
                myRam.flashData_sync.sync_state = DONE_SYNC_FLASH_DATA_TO_SV;
            }
            
            break;
        }
    default:
        break;
    }
}
