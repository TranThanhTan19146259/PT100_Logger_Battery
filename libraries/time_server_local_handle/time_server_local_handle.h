#pragma once

#include "http_client.h"
#include "Ram.h"
#include "ArduinoJson.h"


#define LOCAL_SERVER_NETWORK_PORT 3000
#define LOCAL_SERVER_BASE_URL "/Indr_PT100_Logger"
#define LOCAL_SERVER_TIME_ENDPOINT "/timeData"

void initTimeServerLocal();

void handleTimeServerLocal();