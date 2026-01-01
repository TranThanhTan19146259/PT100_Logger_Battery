#pragma once
#include <Arduino.h>

#define BUFFER_TEMP_DATA_BACKUP_SIZE 2500
#define BUFFER_TIME_DATA_BACKUP_SIZE 2500

typedef struct PT100_data_t
{
    float temp;
    uint16_t resistor;
    uint8_t time_get_data;
} PT100_data;

typedef struct Wifi_config_data
{
    bool wifi_ap_sta; //0: AP || 1: STA 
    bool is_wifi_connected;
    bool reset_wifi;
    String ssid_ap;
    String password_ap;
    String ssid_sta;
    String password_sta;
    String STA_IP;
    String AP_IP;
    String MAC_Address;
} Wifi_config_data_t;

typedef struct Mqtt_config_data
{
    String host;
    uint16_t port;
    String username;
    String password;
    bool isConnectToBroker;
    String devId;
} Mqtt_config_data_t;

typedef struct ntp_time
{
    String ntpTimeString;
    String ntpDateString;
    String ntpDateTimeString;
    struct tm ntpTime;
    bool get_time_ok = 0;
} Ntp_time_t;

typedef enum state_sync_data_backup
{
    FIRST_STARTUP_MEMORY,
    START_ASSIGN_DATA_TO_RAM,
    START_SYNC_RAM_DATA_TO_SV,
    WAIT_TO_SEND_RAM_DATA_TO_SV,
    START_SEND_RAM_DATA_TO_SV,
    CLEAR_RAM_DATA,
    DONE_SYNC_RAM_DATA_TO_SV

} state_sync_data_backup_e;

typedef struct sync_data_sv_backup
{
    float *buf_temp;
    String *buf_time;
    uint32_t time_ptr;
    uint32_t temp_ptr;
    bool is_remaining_data;
    uint32_t mqtt_buff_ptr = 0;;
    state_sync_data_backup_e sync_state;
}sync_data_sv_backup_t;

typedef enum Esp_working_modes{
    ENTER_SLEEP_MODE,
    SLEEP,
    ACTIVE_MODE,
    CONFIG_MODE,
    SEND_LASTWILL_MSG
} Esp_working_modes_e;

typedef struct Esp_working_status
{
    Esp_working_modes_e esp_working_modes;
} Esp_working_status_t;

