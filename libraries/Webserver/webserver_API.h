#ifndef WEBSERVER_API_H
#define WEBSERVER_API_H
#include "ESPAsyncWebServer.h"
#include "flash_read_write.h"
#include <ArduinoJson.h>
#include "str_param_def.h"
#include <esp_log.h>
#include "Ram.h"
#include <EEPROM.h>

#define FW_VERSION "0.0.1"

void on_getReq_rs_dev(AsyncWebServerRequest *request);
void on_getReq_Fw_fs_ver(AsyncWebServerRequest *request);
void on_getReq_all_pt100_data(AsyncWebServerRequest *request);
void on_getReq_add_device_from_mobile_app(AsyncWebServerRequest *request);

void on_postReq_setting(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
void on_postReq_time_send_pt100_data(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
void on_postReq_mqtt_setup(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
void on_postReq_add_device_from_mobile_app(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
void on_postReq_change_pair_key(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
void on_postReq_add_device(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
#endif  

