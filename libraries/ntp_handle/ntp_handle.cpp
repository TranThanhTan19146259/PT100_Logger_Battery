#include "ntp_handle.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600 * GMT;

void init_Ntp()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("configTime() called, waiting for time sync...");
}

void handle_Ntp()
{
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        myRam.ntp_time.get_time_ok = 0;
        return;
    }
    else
    {
        myRam.ntp_time.get_time_ok = 1;
    }
    myRam.ntp_time.ntpTime.tm_year = timeinfo.tm_year;
    myRam.ntp_time.ntpTime.tm_mon  = timeinfo.tm_mon;
    myRam.ntp_time.ntpTime.tm_year += 1900;
    myRam.ntp_time.ntpTime.tm_mon  += 1;
    myRam.ntp_time.ntpTime.tm_mday = timeinfo.tm_mday;
    myRam.ntp_time.ntpTime.tm_sec  = timeinfo.tm_sec;
    myRam.ntp_time.ntpTime.tm_min  = timeinfo.tm_min;
    myRam.ntp_time.ntpTime.tm_hour = timeinfo.tm_hour;
    myRam.ntp_time.ntpTime.tm_hour += GMT;
    static uint32_t t_update_time_data;
    char buf_time_string[50];
    if (millis() - t_update_time_data > 1000)
    {
        ESP_LOGD(TAG, "get time flag: %d", myRam.ntp_time.get_time_ok);
        sprintf(buf_time_string, "%d/%d/%d - %d:%d:%d", myRam.ntp_time.ntpTime.tm_mday, 
                                                        myRam.ntp_time.ntpTime.tm_mon, 
                                                        myRam.ntp_time.ntpTime.tm_year, 
                                                        myRam.ntp_time.ntpTime.tm_hour,
                                                        myRam.ntp_time.ntpTime.tm_min, 
                                                        myRam.ntp_time.ntpTime.tm_sec
                );
        myRam.ntp_time.ntpTimeString = buf_time_string; 
        Serial.println(myRam.ntp_time.ntpTimeString);
        t_update_time_data = millis();
    }
    // if (myRam.wifi_config_data.is_wifi_connected == 1)
    // {
    // }
    
  
}
