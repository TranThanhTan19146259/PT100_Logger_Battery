#pragma once
// #include <Arduino.h>
#include "memory_struct.h"



typedef struct Ram{
    PT100_data_t pt100_data;
    Wifi_config_data_t wifi_config_data;
    Esp_working_status_t working_status;
    Mqtt_config_data_t mqtt_config_data;
    Ntp_time_t ntp_time;
    sync_data_sv_backup_t data_sync;
}Ram_t;

extern Ram_t myRam;
