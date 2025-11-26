#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H
// #include "mesh.h"
#include "webserver_API.h"
#include <ArduinoJson.h>    
#include <esp_wifi.h>

void initProtocol();
void initWifiMode();
void initLocalAPI();
void initWebsource();
void initWebServer();

#endif