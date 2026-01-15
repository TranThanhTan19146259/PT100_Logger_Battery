#pragma once
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <EEPROM.h>
#include "Ram.h"
#include "flash_read_write.h"
#include "str_param_def.h"
#include "sync_data_flash_to_sv.h"

#define MQTT_HOSTNAME "192.168.11.101"
#define MQTT_PORT     1883
#define MQTT_USERNAME "Indr_Pt100"
#define MQTT_PASSWORD "123456789"
// #define PT100_LOGGER_TOPIC_SUB "Indr_PT100/#"
#define PT100_LOGGER_DATA_TOPIC_PUB "PT100_logger/data"
#define PT100_LOGGER_STATUS_TOPIC_PUB "PT100_logger/status"
#define PT100_LOGGER_DATA_DISPLAY_TOPIC_PUB "PT100_logger/data/display"

#define PT100_LOGGER_TB_PUBLISH "v1/devices/me/telemetry"

#define PT100_BASE_MQTT_TOPIC         "Indr_PT100"
#define PT100_MQTT_CONTROL_TOPIC      "control"
#define PT100_SAMPLE_RATE_CMD         "sampleRate"

#define MQTT_SERVER_TIMEOUT_COUNT 5 
#define MQTT_MAX_BUFFER 3000
#define MAX_BUFFER_DATA_POINTS_SEND_TO_SV 30


void initMqtt();
void reconnectMqtt();
void connect_to_broker(char* usr,  char* pass);
void callback(char* topic, byte *payload, unsigned int length);
void handle_mqtt();

void send_data_mqtt(String topic, String jsonData);



