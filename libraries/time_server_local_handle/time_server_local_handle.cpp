#include "time_server_local_handle.h"

void initTimeServerLocal()
{

}


bool checkValidDateTime(struct tm dateTimeInput)
{
    if(dateTimeInput.tm_sec  < 60 && dateTimeInput.tm_sec  >= 0 && 
       dateTimeInput.tm_min  < 60 && dateTimeInput.tm_min  >= 0 && 
       dateTimeInput.tm_hour < 60 && dateTimeInput.tm_hour >= 0)
    {
        return 1;
    }
    return 0;
}

void handleTimeServerLocal()
{
    String localServer_hostName = "http://" + myRam.mqtt_config_data.host + ":" + String(LOCAL_SERVER_NETWORK_PORT) + String(LOCAL_SERVER_BASE_URL);
    static uint32_t t_get_localServer_time;
    if (myRam.wifi_config_data.wifi_ap_sta == 1)
    {
        if (millis() - t_get_localServer_time > 1000)
        {
            String res;
            getReqHttp(localServer_hostName, LOCAL_SERVER_TIME_ENDPOINT, res);
            Serial.println(res);
            t_get_localServer_time = millis();

            StaticJsonDocument<64> doc;

            DeserializationError error = deserializeJson(doc, res);

            if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            myRam.localServer_time.get_time_ok = 0;
            // return;
            }
            else
            {
                const char* time = doc["time"]; // "22:49:30"
                const char* date = doc["date"]; // "11/01/2026"
                uint16_t time_arr[3];
                uint16_t date_arr[3];
                sscanf((char*) time, "%d:%d:%d", &time_arr[0], &time_arr[1], &time_arr[2]);
                sscanf((char*) date, "%d/%d/%d", &date_arr[0], &date_arr[1], &date_arr[2]);
                myRam.localServer_time.localServerTime.tm_year = date_arr[2];
                myRam.localServer_time.localServerTime.tm_mon  = date_arr[1];
                myRam.localServer_time.localServerTime.tm_mday = date_arr[0];

                myRam.localServer_time.localServerTime.tm_sec  = time_arr[2];
                myRam.localServer_time.localServerTime.tm_min  = time_arr[1];
                myRam.localServer_time.localServerTime.tm_hour = time_arr[0];
                myRam.localServer_time.localServerDateTimeString = String(date) + " " + String(time);
            }
            
            char buf_time_string[50];
            char buf_date_string[50];
            
            // ESP_LOGD(TAG, "get time flag: %d", myRam.localServer_time.get_time_ok);
            sprintf(buf_date_string, "%d/%d/%d",            myRam.localServer_time.localServerTime.tm_mday,
                                                            myRam.localServer_time.localServerTime.tm_mon, 
                                                            myRam.localServer_time.localServerTime.tm_year
                    );
            
            
            sprintf(buf_time_string, "%d:%d:%d",            myRam.localServer_time.localServerTime.tm_hour,
                                                            myRam.localServer_time.localServerTime.tm_min, 
                                                            myRam.localServer_time.localServerTime.tm_sec
                    );
            myRam.localServer_time.localServerTimeString = buf_time_string; 
            myRam.localServer_time.localServerDateString = buf_date_string; 

            myRam.localServer_time.get_time_ok = checkValidDateTime(myRam.localServer_time.localServerTime);

            

            Serial.println("---------------------------------------------------");
            Serial.println(myRam.localServer_time.localServerTimeString);
            Serial.println(myRam.localServer_time.localServerDateString);
            Serial.println(myRam.localServer_time.localServerDateTimeString);
            Serial.println("---------------------------------------------------");
            // Serial.println(myRam.localServer_time.localServerDateString);
        }
    }
    
    
}
