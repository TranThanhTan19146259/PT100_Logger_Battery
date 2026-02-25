#pragma once

#include <SPI.h>
#include <SPIFlash.h>
#include "Ram.h"
#include "Rom.h"
#define TOTAL_SECTORS 4096
// #define SECTOR_SIZE 4096 // 4KB
// #define TOTAL_SECTORS 5
#define SECTOR_SIZE 4096 // 4KB
#define TIME_SYNC_FLASH_TO_SV 2000// ms

uint32_t convertHumanDateToUnixTime(String dateTime);
void convertUnixTimeToHumanDate(uint32_t unixTime, struct tm *output_tm);
void read_pt100_flash(uint32_t addr, uint64_t &U64Data);

void init_sync_flashData();
void handle_sync_flashData();
void save_data_offline_to_flash(String dateTimeString);
void save_data_offline_to_flash_with_time(String timeSnapShot);
