#pragma once
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <EEPROM.h>
#include "Ram.h"
#include "flash_read_write.h"
#include "str_param_def.h"

#define MQTT_HOSTNAME "192.168.11.101"
#define MQTT_PORT     1883
#define MQTT_USERNAME "Indr_Pt100"
#define MQTT_PASSWORD "123456789"
#define PT100_LOGGER_TOPIC_SUB "PT100_logger/#"
#define PT100_LOGGER_DATA_TOPIC_PUB "PT100_logger/data"
#define PT100_LOGGER_STATUS_TOPIC_PUB "PT100_logger/status"

#define PT100_LOGGER_TB_PUBLISH "v1/devices/me/telemetry"

#define PT100_BASE_MQTT_TOPIC         "PT100_logger"
#define PT100_CONFIG_BASE_MQTT_TOPIC  "config"
#define PT100_INFO_BASE_MQTT_TOPIC    "info"
#define PT100_CONFIG_TIMESEND_CMD     "timeSend"
#define PT100_STATUS_CMD              "status"

#define MQTT_SERVER_TIMEOUT_COUNT 5


void initMqtt();
void reconnectMqtt();
void connect_to_broker(char* usr,  char* pass);
void callback(char* topic, byte *payload, unsigned int length);
void handle_mqtt();

void send_data_mqtt(String topic, String jsonData);



