#pragma once
#include <Wire.h>
#include <RTClib.h>
#include "Ram.h"
#include "sync_data_flash_to_sv.h"
#include "mqtt_handle.h"
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Define your custom I2C pins
#define I2C_SDA 26
#define I2C_SCL 25
#define SQWinput 14

void init_rtc_ds3231();

void handle_rtc_ds3231();
