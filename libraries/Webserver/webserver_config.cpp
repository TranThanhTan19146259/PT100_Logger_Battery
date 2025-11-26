#include "webserver_config.h"
#include "Update.h"

AsyncWebServer server(80);


void initWifiMode()
{
  setCpuFrequencyMhz(80);
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  String buf, buf2, buf3;
  readString_from_spiffs("/config_device.json", buf);
  ESP_LOGD(TAG,"%s",buf);
  StaticJsonDocument<250> doc;

  DeserializationError error = deserializeJson(doc, buf);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  bool mode_ap_sta = doc["mode_AP_STA"];
  const char* ssid = doc["ssid"][mode_ap_sta];
  const char* password = doc["password"][mode_ap_sta];
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(mode_ap_sta);
  delay(1000);

  const char* ssid_ap = doc["ssid"][0];
  const char* password_ap = doc["password"][0];
  const char* ssid_sta = doc["ssid"][1];
  const char* password_sta = doc["password"][1];
  String ssid_ap_str = ssid_ap; 
  String password_ap_str = password_ap; 
  String ssid_sta_str = ssid_sta; 
  String password_sta_str = password_sta; 
  String mac_address_str;



  myRam.wifi_config_data.wifi_ap_sta = mode_ap_sta;
  myRam.wifi_config_data.password_ap = password_ap;
  myRam.wifi_config_data.ssid_ap = ssid_ap;
  myRam.wifi_config_data.password_sta = password_sta;
  myRam.wifi_config_data.ssid_sta = ssid_sta;
if (mode_ap_sta)
{
  /* code */
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid_sta, password_sta);
  char mac_address[20];
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  sprintf(mac_address, "%02X%02X%02X%02X%02X%02X",  
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  myRam.wifi_config_data.MAC_Address = mac_address;
  mac_address_str = mac_address;
  // while (WiFi.status() != WL_CONNECTED) {
  //     delay(1000);
      // config_wifi_mode();
  // ESP_LOGD(TAG,"Connecting to WiFi..\n");
  // }
  // ESP_LOGD(TAG,"Done");
}
else
{
  IPAddress local_IP(192, 168, 10, 100);
  IPAddress gateway(192, 168, 10, 1);

  IPAddress subnet(255, 255, 255, 0);
  char mac_address[20];
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_SOFTAP);
  sprintf(mac_address, "_%02X%02X%02X%02X%02X%02X",  
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  mac_address_str = mac_address;
  ESP_LOGD(ESP_LOGD,"%s",mac_address_str.c_str());
  WiFi.mode(WIFI_AP); 
  ssid_ap_str += mac_address_str;
  Serial.println(ssid_ap_str);
  WiFi.softAP(ssid_ap_str.c_str(), password_ap_str.c_str());
  delay(1000);
  if (!WiFi.softAPConfig(local_IP,gateway,subnet))
  {
    Serial.println("AP Failed to configure");
  }
  
}
}

void initLocalAPI()
{
  server.on("/rs_device", HTTP_GET, on_getReq_rs_dev);
  server.on("/get_fw_fs_version", HTTP_GET, on_getReq_Fw_fs_ver);
  server.on("/pt100_data", HTTP_GET, on_getReq_all_pt100_data);
  // server.on("/add_new_device_from_mobile_app",HTTP_GET,on_getReq_add_device_from_mobile_app);
  server.on("/setting_tab",HTTP_POST,[](AsyncWebServerRequest * request){}, NULL,on_postReq_setting);
  server.on("/pt100_data_timeSend",HTTP_POST,[](AsyncWebServerRequest * request){}, NULL,on_postReq_time_send_pt100_data);
  server.on("/config_mqtt",HTTP_POST,[](AsyncWebServerRequest * request){}, NULL,on_postReq_mqtt_setup);
  
  // server.on("/add_new_device",HTTP_POST,[](AsyncWebServerRequest * request){}, NULL,on_postReq_add_device);
  // server.on("/add_new_device_from_mobile_app",HTTP_POST,[](AsyncWebServerRequest * request){}, NULL,on_postReq_add_device_from_mobile_app);
  // server.on("/change_pair_key",HTTP_POST,[](AsyncWebServerRequest * request){}, NULL,on_postReq_change_pair_key);
  
}

void initWebsource()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
  request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on(PSTR("/resources/css/style.css"), HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/resources/css/style.css", "text/css");
  });

  server.on(PSTR("/resources/js/script.js"), HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/resources/js/script.js", "text/javascript");
  });

  server.on(PSTR("/resources/js/con2sv.js"), HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/resources/js/con2sv.js", "text/javascript");
  });

  server.on(PSTR("/resources/js/settingConf.js"), HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/resources/js/settingConf.js", "text/javascript");
  });

  server.on(PSTR("/resources/js/nwConf.js"), HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/resources/js/nwConf.js", "text/javascript");
  });

  server.on(PSTR("/resources/js/reqMacnType.js"), HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/resources/js/reqMacnType.js", "text/javascript");
  });
  server.on(
    PSTR("/update_fw"), HTTP_POST,
    [](AsyncWebServerRequest *request)
    {
        AsyncWebServerResponse *response = request->beginResponse((Update.hasError()) ? 500 : 200, "text/plain", (Update.hasError()) ? "FAIL" : "DONE");
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
        /// hardcode
        if (Update.hasError() == false)
        {
            yield();
            delay(1000);
            yield();
            ESP.restart();
        }
        /// hardcode
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
    {
        // if(!request->authenticate(http_username, http_password))
        // {
        //     yield();
        //     // ESP_LOGD(TAG, "request authentication");
        //     return request->requestAuthentication();
        // }

        if (!index)
        {
            int cmd = U_FLASH;
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
            { // Start with max available size
                // Update.printError(Serial);
                return request->send(400, "text/plain", "OTA could not begin");
            }
        }
        if (len)
        {
            if (Update.write(data, len) != len)
            {
                return request->send(400, "text/plain", "OTA could not begin");
            }
        }
        if (final)
        { // if the final flag is set then this is the last frame of data
            if (!Update.end(true))
            { // true to set the size to the current progress
                Update.printError(Serial);
                return request->send(400, "text/plain", "Could not end OTA");
            }
        }
        else
        {
            return;
        }
        // ESP_LOGD(TAG, "%s", filename);
        // ESP_LOGD(TAG, "%d - %ld\n", index, len);
        // if(!index)
        // {
        //     for(size_t i =0; i < len; i++)
        //     {
        //         // ESP_LOGD(TAG, "0x%02X - %c", data[i], data[i]);
        //     }
        // }
        // if(final)
        // {
        //     ESP_LOGD(TAG, "final file");
        // }
    });
  server.on(
    PSTR("/update_fs"), HTTP_POST,
    [](AsyncWebServerRequest *request)
    {
        AsyncWebServerResponse *response = request->beginResponse((Update.hasError()) ? 500 : 200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
        /// hardcode
        if (Update.hasError() == false)
        {
            yield();
            delay(1000);
            yield();
            ESP.restart();
        }
        /// hardcode
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
    {
        // if(!request->authenticate(http_username, http_password))
        // {
        //     yield();
        //     // ESP_LOGD(TAG, "request authentication");
        //     return request->requestAuthentication();
        // }

        if (!index)
        {
            int cmd = U_SPIFFS;
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
            { // Start with max available size
                // Update.printError(Serial);
                return request->send(400, "text/plain", "OTA could not begin");
            }
        }
        if (len)
        {
            if (Update.write(data, len) != len)
            {
                return request->send(400, "text/plain", "OTA could not begin");
            }
        }
        if (final)
        { // if the final flag is set then this is the last frame of data
            if (!Update.end(true))
            { // true to set the size to the current progress
                Update.printError(Serial);
                return request->send(400, "text/plain", "Could not end OTA");
            }
        }
        else
        {
            return;
        }
        // ESP_LOGD(TAG, "%s", filename);
        // ESP_LOGD(TAG, "%d - %ld\n", index, len);
        // if(!index)
        // {
        //     for(size_t i =0; i < len; i++)
        //     {
        //         // ESP_LOGD(TAG, "0x%02X - %c", data[i], data[i]);
        //     }
        // }
        // if(final)
        // {
        //     ESP_LOGD(TAG, "final file");
        // }
    });

}

// void initProtocol()
// {
//   initMqtt();

//   // uint8_t data;
//   // read_byte_EEPROM(0,&data);
//   // // Serial.println(data);
//   // //Choose network protocol mode in EEPROM 
//   // switch (data)
//   // {
//   // case 0:
//   //   break;
//   // case 1:
//   //   {
//   //     initMqtt();
//   //   }
//   //   break;
//   // default:
//   //   break;
//   // }
// }

void initWebServer()
{
  initWifiMode(); //#1
  initWebsource(); //#2
  initLocalAPI(); //#3
  // initProtocol(); //#4
  server.begin();
}
