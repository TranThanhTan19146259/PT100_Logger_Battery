#pragma once
#include <Arduino.h>

#define BUFFER_TEMP_DATA_BACKUP_SIZE 2500
#define BUFFER_TIME_DATA_BACKUP_SIZE 2500

typedef struct PT100_data_t
{
    float temp;
    uint16_t resistor;
    uint8_t time_get_data;
    uint8_t sampleRate;
    uint64_t tempTimeRawData; // this data is used for sotring into flash
    bool timer_tick;
    uint8_t reset_times;
} PT100_data;

typedef struct Wifi_config_data
{
    volatile bool wifi_ap_sta; //0: AP || 1: STA 
    volatile bool is_wifi_connected;
    volatile bool reset_wifi;
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
    volatile bool mqttConnection;
    String devId;
    bool mqtt_tick;
    String dateTimeLastestBuffer;
} Mqtt_config_data_t;

typedef enum Mqtt_state
{
    FIRST_START_UP,
    SENT_DATA_TO_SV,
    WAIT_FOR_DATA_RESPONSE,
    DONE_HANDLE_SENT_MSG,
} Mqtt_state_e;

typedef struct ntp_time
{
    String ntpTimeString;
    String ntpDateString;
    String ntpDateTimeString;
    struct tm ntpTime;
    bool get_time_ok = 0;
} Ntp_time_t;

typedef struct LocalServer_time
{
    String localServerTimeString;
    String localServerDateString;
    String localServerDateTimeString;
    struct tm localServerTime;
    bool get_time_ok = 0;
} LocalServer_time_t;


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


typedef enum state_sync_FlashData_backup
{
    FIRST_STARTUP_FLASH_MEMORY,
    START_ASSIGN_DATA_TO_FLASH,
    START_SYNC_FLASH_DATA_TO_SV,
    WAIT_TO_SEND_FLASH_DATA_TO_SV,
    START_SEND_FLASH_DATA_TO_SV,
    CLEAR_FLASH_DATA,
    WAIT_FOR_CLEARING_FLASH_DATA,
    DONE_SYNC_FLASH_DATA_TO_SV

} state_sync_FlashData_backup_e;

// typedef struct sync_flash_queue
// {
//     uint8_t q_adrr_tail;
//     uint8_t q_adrr_head;
// } sync_flash_queue_t;


typedef struct sync_Flashdata_sv_backup
{
    uint32_t flashAddrTail = 0x0000; // get current flash address to detect sector and flash address to write data
    uint32_t flashAddrHead = 0x0000; // get current flash address to detect sector and flash address to write data
    state_sync_FlashData_backup_e sync_state;
    uint32_t total_sector_used;
    uint64_t total_offline_data_stored;
    uint32_t ptr_buf_flash_offline_data;
    bool response_from_server = 0; // offline data stream
    bool response_from_server_for_lastestData = 0; // online data stream
    bool flash_init_ok = 0;
    volatile bool flash_save_data_tick;
} sync_Flashdata_sv_backup_t;

typedef struct Rtc_time
{
    String rtcTimeString;
    String rtcDateString;
    String rtcDateTimeString;
    struct tm rtcTime;
    bool rtc_tick;
    bool init_ok;
} Rtc_time_t;

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

