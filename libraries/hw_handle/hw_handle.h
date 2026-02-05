#pragma once

#include "network_handle.h"
#include "Ram.h"
#include "Rom.h"
#include "soc/rtc_wdt.h"
#include "ntp_handle.h"
#include "sync_data_flash_to_sv.h"
#include <Adafruit_MAX31865.h>
#include "rtc_ds3231_pt100.h"
// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
#define RNOMINAL  100.0
// #define CONFIG_BTN 13
#define CONFIG_BTN 26
#define ADC_PIN 32
#define MCU_FEQ 240// 80 MHz
#define OFFSET_SAMPLE_RATE 50 // ms

#define SIMULATE_DATA


void init_hw();

void handle_hw();