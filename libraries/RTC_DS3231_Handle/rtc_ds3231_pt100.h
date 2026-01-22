#pragma once
#include <Wire.h>
#include <RTClib.h>
#include "Ram.h"
#include <time.h>


// Define your custom I2C pins
#define I2C_SDA 26
#define I2C_SCL 25

void init_rtc_ds3231();

void handle_rtc_ds3231();
