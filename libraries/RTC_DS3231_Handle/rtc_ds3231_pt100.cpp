#include "rtc_ds3231_pt100.h"
RTC_DS3231 rtc;

int8_t compare_two_time_sources(struct tm *timeSource1, struct tm *timeSource2)
{
    time_t t1 = mktime(timeSource1);
    time_t t2 = mktime(timeSource2);
    
    if (t1 < t2) return -1;
    if (t1 > t2) return 1;
    return 0;  // equal
}

bool timeToRead = false;

void IRAM_ATTR sqwISR() {
  timeToRead = true;
}

void init_rtc_ds3231()
{
    Wire.begin(I2C_SDA, I2C_SCL);
    // yield();
    if (!rtc.begin())
    {
        myRam.rtc_time.init_ok = 0;
    }
    // Configure DS3231 SQW pin for 1Hz output
    // This will generate an interrupt every 1 second
    rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
    
    // Setup interrupt pin
    pinMode(SQWinput, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SQWinput), sqwISR, FALLING);
    delay(5000);
    if (rtc.lostPower())
    {
        ESP_LOGD(TAG, "rtc lost power");
    }
    
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // else
    // {
    //     myRam.rtc_time.init_ok = 1;
    // }
}

DateTime now;
void handle_rtc_ds3231()
{
    static uint32_t t_print_time;
    
    static uint32_t last_sec = 0;
    static uint8_t secondCounter = 0; 

    char buf_date_string[50];
    char buf_time_string[50];

    if (now.isValid())
    {
        if (timeToRead) 
        {
            timeToRead = false;
            secondCounter++;
            // Print time every N seconds

            if (secondCounter >= myRam.pt100_data.sampleRate) {
                myRam.rtc_time.rtc_tick = 1;
                // myRam.rtc_time.rtcTime.tm_year = now.year(); 
                // myRam.rtc_time.rtcTime.tm_mon  = now.month(); 
                // myRam.rtc_time.rtcTime.tm_mday = now.day(); 
                // myRam.flashData_sync.flash_save_data_tick = 1;
                now = rtc.now();
                ESP_LOGD(TAG,"DAY: %d day ram: %d FLASH_TICK %d", now.day(), myRam.rtc_time.rtcTime.tm_mday,myRam.flashData_sync.flash_save_data_tick);
                    sprintf(buf_date_string, "%d/%d/%d",            now.day(),
                                                                    now.month(), 
                                                                    now.year()
                );
                myRam.rtc_time.rtcDateString = buf_date_string; 
                myRam.rtc_time.rtcTime.tm_hour = now.hour(); 
                myRam.rtc_time.rtcTime.tm_min  = now.minute(); 
                myRam.rtc_time.rtcTime.tm_sec  = now.second();

                // ESP_LOGD(TAG, "get time flag: %d", myRam.ntp_time.get_time_ok);
                sprintf(buf_time_string, "%d:%d:%d",    myRam.rtc_time.rtcTime.tm_hour,
                                                        myRam.rtc_time.rtcTime.tm_min, 
                                                        myRam.rtc_time.rtcTime.tm_sec
                );
                // myRam.mqtt_config_data.mqtt_tick = 1;
                secondCounter = 0;
            }
        }
        
    }
    

    if (myRam.wifi_config_data.is_wifi_connected)
    {
        if (compare_two_time_sources(&myRam.rtc_time.rtcTime, &myRam.ntp_time.ntpTime) != 0 && myRam.ntp_time.get_time_ok)
        {
            // start synchronizing time source server to stc
            myRam.rtc_time.rtcTime.tm_hour = myRam.ntp_time.ntpTime.tm_hour; 
            myRam.rtc_time.rtcTime.tm_min  = myRam.ntp_time.ntpTime.tm_min; 
            myRam.rtc_time.rtcTime.tm_sec  = myRam.ntp_time.ntpTime.tm_sec;

            myRam.rtc_time.rtcTime.tm_year = myRam.ntp_time.ntpTime.tm_year; 
            myRam.rtc_time.rtcTime.tm_mon  = myRam.ntp_time.ntpTime.tm_mon; 
            myRam.rtc_time.rtcTime.tm_mday = myRam.ntp_time.ntpTime.tm_mday; 
            DateTime dt_set(myRam.ntp_time.ntpTime.tm_year, 
                            myRam.ntp_time.ntpTime.tm_mon,
                            myRam.ntp_time.ntpTime.tm_mday,
                            myRam.ntp_time.ntpTime.tm_hour, 
                            myRam.ntp_time.ntpTime.tm_min,
                            myRam.ntp_time.ntpTime.tm_sec
            );
            rtc.adjust(dt_set);
            ESP_LOGD(TAG, "-----------------START SYNC DATA--------------------");
        }
    }




    myRam.rtc_time.rtcTimeString = buf_time_string; 
    myRam.rtc_time.rtcDateTimeString = String(buf_date_string) + " " + String(buf_time_string);  
    myRam.rtc_time.init_ok = now.isValid();

    if (myRam.rtc_time.rtc_tick == 1)
    {
        ESP_LOGD(TAG, "valid time: %d", now.isValid());
        Serial.println(myRam.rtc_time.rtcTimeString);
        Serial.println(myRam.rtc_time.rtcDateString);
        Serial.println(myRam.rtc_time.rtcDateTimeString);
        static uint32_t t_save_offline_data;
        Serial.println("---------------------------TICK TEST--------------------------------");
        ESP_LOGD(TAG,"---------------------------TICK TEST--------------------------------");
        ESP_LOGD(TAG,"STATE %d", myRam.flashData_sync.sync_state);
        ESP_LOGD(TAG,"ESP RAM MEMORY: %d reset count %d", ESP.getFreeHeap(), myRam.pt100_data.reset_times);
        // ESP_LOGD(TAG,"MQTT TICK: %d FLASH TICK %d", myRam.mqtt_config_data.mqtt_tick, myRam.flashData_sync.flash_save_data_tick);
        // ESP_LOGD(TAG,"MQTT Connect: %d", myRam.mqtt_config_data.mqtt_tick, myRam.flashData_sync.flash_save_data_tick);
        if (myRam.flashData_sync.sync_state == START_ASSIGN_DATA_TO_FLASH)
        {
            Serial.println("START ASSIGNING DATA");
            ESP_LOGD(TAG, "START ASSIGNING DATA");
            save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
            // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
        }
        send_lastestData_to_sv(myRam.rtc_time.rtcDateTimeString);
        if (myRam.mqtt_state == WAIT_FOR_DATA_RESPONSE)
        {
            {
                ESP_LOGD(TAG,"Wait for response mqtt %d", myRam.flashData_sync.response_from_server_for_lastestData);
                // had response from server
                if (myRam.flashData_sync.response_from_server_for_lastestData == 0)
                {
                    // myRam.flashData_sync.response_from_server_for_lastestData = 0;
                    ESP_LOGD(TAG,"No data response => save data to flash");
                    // save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
                }
                else
                {
                    
                }
                myRam.flashData_sync.response_from_server_for_lastestData = 0;
                myRam.mqtt_state = DONE_HANDLE_SENT_MSG;
            }
        }
        // myRam.flashData_sync.flash_save_data_tick = 1;
        myRam.rtc_time.rtc_tick = 0;
    }
}