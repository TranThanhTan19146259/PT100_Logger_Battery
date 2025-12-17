#pragma once
#include <Arduino.h>

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
} Mqtt_config_data_t;



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

