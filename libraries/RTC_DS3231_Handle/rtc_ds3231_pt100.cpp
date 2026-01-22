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

void init_rtc_ds3231()
{
    Wire.begin(I2C_SDA, I2C_SCL);

    if (!rtc.begin())
    {
        myRam.rtc_time.init_ok = 0;
    }
    else
    {
        myRam.rtc_time.init_ok = 1;
    }
}

void handle_rtc_ds3231()
{
    DateTime now = rtc.now();
    static uint32_t t_print_time;
    myRam.rtc_time.rtcTime.tm_hour = now.hour(); 
    myRam.rtc_time.rtcTime.tm_min  = now.minute(); 
    myRam.rtc_time.rtcTime.tm_sec  = now.second();

    myRam.rtc_time.rtcTime.tm_year = now.year(); 
    myRam.rtc_time.rtcTime.tm_mon  = now.month(); 
    myRam.rtc_time.rtcTime.tm_mday = now.day(); 

    // struct tm timeRaw;
    // timeRaw.tm_hour = 10;
    // timeRaw.tm_min  = 10;
    // timeRaw.tm_sec  = 10;

    // timeRaw.tm_year  = 2026;
    // timeRaw.tm_mon   = 10;
    // timeRaw.tm_mday  = 10;
    if (compare_two_time_sources(&myRam.rtc_time.rtcTime, &myRam.ntp_time.ntpTime) != 0 && myRam.ntp_time.get_time_ok)
    {
        // start synchronizing time source server to stc
        myRam.rtc_time.rtcTime.tm_hour = myRam.ntp_time.ntpTime.tm_hour; 
        myRam.rtc_time.rtcTime.tm_min  = myRam.ntp_time.ntpTime.tm_min; 
        myRam.rtc_time.rtcTime.tm_sec  = myRam.ntp_time.ntpTime.tm_sec;

        myRam.rtc_time.rtcTime.tm_year = myRam.ntp_time.ntpTime.tm_year; 
        myRam.rtc_time.rtcTime.tm_mon  = myRam.ntp_time.ntpTime.tm_mon; 
        myRam.rtc_time.rtcTime.tm_mday = myRam.ntp_time.ntpTime.tm_mday; 
        DateTime dt_set(myRam.rtc_time.rtcTime.tm_year, 
                        myRam.rtc_time.rtcTime.tm_mon,
                        myRam.rtc_time.rtcTime.tm_mday,
                        myRam.rtc_time.rtcTime.tm_hour, 
                        myRam.rtc_time.rtcTime.tm_min,
                        myRam.rtc_time.rtcTime.tm_sec
        );
        rtc.adjust(dt_set);
    }
    char buf_time_string[50];
    char buf_date_string[50];
    // ESP_LOGD(TAG, "get time flag: %d", myRam.ntp_time.get_time_ok);
    sprintf(buf_time_string, "%d:%d:%d",            myRam.rtc_time.rtcTime.tm_hour,
                                                    myRam.rtc_time.rtcTime.tm_min, 
                                                    myRam.rtc_time.rtcTime.tm_sec
            );
    sprintf(buf_date_string, "%d/%d/%d",            myRam.rtc_time.rtcTime.tm_mday,
                                                    myRam.rtc_time.rtcTime.tm_mon, 
                                                    myRam.rtc_time.rtcTime.tm_year
            );
    myRam.rtc_time.rtcTimeString = buf_time_string; 
    myRam.rtc_time.rtcDateString = buf_date_string; 
    myRam.rtc_time.rtcDateTimeString = String(buf_date_string) + " " + String(buf_time_string);  
    if (millis() - t_print_time  > 1000)
    {
        Serial.println(myRam.rtc_time.rtcTimeString);
        Serial.println(myRam.rtc_time.rtcDateString);
        Serial.println(myRam.rtc_time.rtcDateTimeString);
        t_print_time = millis();
    }
    
}