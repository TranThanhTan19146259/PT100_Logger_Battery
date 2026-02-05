#pragma once
// #include <Arduino.h>
#include "memory_struct.h"

template <typename T>
union convert_data{
  uint8_t unmarshall[sizeof(T)];
  T marshall;
};


typedef struct Ram{
    PT100_data_t pt100_data;
    Wifi_config_data_t wifi_config_data;
    Esp_working_status_t working_status;
    Mqtt_config_data_t mqtt_config_data;
    Mqtt_state_e mqtt_state;
    Ntp_time_t ntp_time;
    LocalServer_time localServer_time;
    sync_Flashdata_sv_backup_t flashData_sync;
    Rtc_time_t rtc_time;
}Ram_t;

extern Ram_t myRam;
