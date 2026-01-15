#include "mqtt_handle.h"
WiFiClient wifiClient;
PubSubClient client(wifiClient);


void callback(char* topic, byte *payload, unsigned int length)
{
    char status[20];
    // Serial.println("-------new message from broker-----");
    // Serial.print("topic: ");
    // Serial.println(topic);
    
    //parse topic 
    char *str = (char *)topic;
    const char s[2] = "/";
    char *token;
    char buf[5][20];
    token = strtok(str, s);
    int i = 0;
    while (token != NULL)
    {
      strcpy(buf[i], token);
      token = strtok(NULL, s);
      i++;
    }
    Serial.println(buf[0]);
    Serial.println(buf[1]);
    Serial.println(buf[2]);
    if (memcmp(buf[0], PT100_BASE_MQTT_TOPIC, strlen(PT100_BASE_MQTT_TOPIC)) == 0)
    {
      ESP_LOGD(TAG,"correct base!");
      if (memcmp(buf[1], myRam.mqtt_config_data.devId.c_str(), strlen(myRam.mqtt_config_data.devId.c_str())) == 0)
      {
        ESP_LOGD(TAG,"correct devId!");
        if (memcmp(buf[2], PT100_MQTT_CONTROL_TOPIC, strlen(PT100_MQTT_CONTROL_TOPIC)) == 0)
        {
          ESP_LOGD(TAG,"correct control!");
          String payload_str;
          for (uint8_t i = 0; i < length; i++)
          {
            payload_str += (char)payload[i];
          }
          Serial.println(payload_str); 
          StaticJsonDocument<128> doc;
          DeserializationError error = deserializeJson(doc, payload_str);

          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
          }
          const char* cmd = doc["cmd"]; // "sampleRate"
          int value = doc["value"]; // 50
          const char* status = doc["status"]; // check this data come from device or server, if data com from server there will be no "status" data
          Serial.print("status ");
          Serial.println(status);
          if (status == nullptr || *status == '\0')
          {
            if (memcmp(cmd, PT100_SAMPLE_RATE_CMD, strlen(PT100_SAMPLE_RATE_CMD)) == 0)
            {
              myRam.pt100_data.sampleRate = value;
              EEPROM.write(1, value);
              EEPROM.commit();
              String output;
              StaticJsonDocument<128> doc;
              doc["cmd"] = PT100_SAMPLE_RATE_CMD;
              doc["value"] = myRam.pt100_data.sampleRate;
              doc["status"] = "ok";
              serializeJson(doc, output);
              String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/" + PT100_MQTT_CONTROL_TOPIC;
              send_data_mqtt(mqtt_topic_data, output);
            }
          }
          
          
        }
      }
      
      
    }



}
bool isConnectedBroker;

void connect_to_broker(char *usr, char *pass)
{
  static uint8_t disc_to_sv_counter;
  if (!client.connected())
  {

    Serial.print("Attempting MQTT connection...");
    String clientId = "PT100_Logger";
    clientId += myRam.wifi_config_data.MAC_Address;

    if (client.connect(clientId.c_str(), usr, pass))
    {
      isConnectedBroker = 1;
      disc_to_sv_counter = 0;
      Serial.println("connected");
      char buf_sub_topic[100];
      sprintf(buf_sub_topic,"%s/%s/control",PT100_BASE_MQTT_TOPIC, myRam.mqtt_config_data.devId.c_str());
      client.subscribe(buf_sub_topic);

    }
    else
    {
      isConnectedBroker = 0;
      disc_to_sv_counter++;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
    }
    //check how many times device disconnect to server
    if (disc_to_sv_counter > MQTT_SERVER_TIMEOUT_COUNT)
    {
      // myRam.working_status.esp_working_modes = ENTER_SLEEP_MODE;
    }
    
  }
}

void reconnectMqtt()
{
  static unsigned long t;
  if (millis() - t > 1000)
  {
    connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
    // connect_to_broker(MQTT_USERNAME, MQTT_PASSWORD);
    client.loop();
    t = millis();
  }
}

void send_data_mqtt(String topic, String jsonData)
{
  static unsigned long t;
  reconnectMqtt(); // use when device disconnect to server
  const char *data = jsonData.c_str();
  client.publish(topic.c_str(), data); // use to publish data via mqtt
}

void initMqtt()
{
  String output;
  readString_from_spiffs(config_mqtt_protocol, output);
  Serial.println(output);
  StaticJsonDocument<192> doc;

  DeserializationError error = deserializeJson(doc, output);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* host = doc["host"]; // "192.168.11.101"
  const char* port = doc["port"]; // "1883"
  const char* username = doc["username"]; // "Indr_Pt100"
  const char* password = doc["password"]; // "123456789"
  const char* devId = doc["devId"]; // "123456789"

  uint16_t _port = atoi(port);
  ESP_LOGD(TAG,"host: %s\nport: %d\nusername: %s\npassword: %s\n",host, _port, username, password);
  myRam.mqtt_config_data.host = host; 
  myRam.mqtt_config_data.port = _port; 
  myRam.mqtt_config_data.username = username; 
  myRam.mqtt_config_data.password = password; 
  myRam.mqtt_config_data.devId = devId; 
  client.setServer(myRam.mqtt_config_data.host.c_str(), myRam.mqtt_config_data.port);
  client.setCallback(callback);
  client.setBufferSize(MQTT_MAX_BUFFER);
  connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
}


void convert_flashData_to_timeTempData(uint32_t addr, float &tempDataOuput, String &timeDataOutput)
{
    uint64_t dataU64;
    read_pt100_flash(addr, dataU64);
    // ESP_LOGD(TAG, "Addr %d Mqtt Data R = 0x%016" PRIx64, (i * 8) ,dataU64);
    convert_data<uint64_t> timeTempDataRaw;
    timeTempDataRaw.marshall = dataU64;
    convert_data<float> tempData;
    tempData.unmarshall[0] = timeTempDataRaw.unmarshall[4];
    tempData.unmarshall[1] = timeTempDataRaw.unmarshall[5];
    tempData.unmarshall[2] = timeTempDataRaw.unmarshall[6];
    tempData.unmarshall[3] = timeTempDataRaw.unmarshall[7];
    convert_data<uint32_t> unixTimeStamp;
    unixTimeStamp.unmarshall[0] = timeTempDataRaw.unmarshall[0];
    unixTimeStamp.unmarshall[1] = timeTempDataRaw.unmarshall[1];
    unixTimeStamp.unmarshall[2] = timeTempDataRaw.unmarshall[2];
    unixTimeStamp.unmarshall[3] = timeTempDataRaw.unmarshall[3];
    tempDataOuput = tempData.marshall;
    char buf_time_data[30];
    
    struct tm timeInfo;
    convertUnixTimeToHumanDate(unixTimeStamp.marshall, &timeInfo);
    sprintf(buf_time_data, "%d/%d/%d %d:%d:%d", timeInfo.tm_mday,timeInfo.tm_mon,timeInfo.tm_year,
                                                timeInfo.tm_hour,timeInfo.tm_min,timeInfo.tm_sec
    );
    timeDataOutput = buf_time_data;
    // ESP_LOGD(TAG,"temp: %.2f time: %s", tempData.marshall, timeDataOutput.c_str());
}


void handle_mqtt()
{
  reconnectMqtt();
  static uint32_t t_send_data = 0;
  static uint32_t t_bk_data = 0;
  static uint8_t state_generate_buf_temp_time_bk;  
  // if ((isConnectedBroker == 0 || myRam.wifi_config_data.is_wifi_connected == 0) && myRam.localServer_time.get_time_ok == 1) // use this condition for local server time
  if ((isConnectedBroker == 0 || myRam.wifi_config_data.is_wifi_connected == 0) && myRam.ntp_time.get_time_ok == 1) // use this condition for ntp server time
  {
    myRam.mqtt_config_data.isConnectToBroker = 0;
    // myRam.data_sync.sync_state = START_ASSIGN_DATA_TO_RAM;
    myRam.flashData_sync.sync_state = START_ASSIGN_DATA_TO_FLASH;
  }
  if (millis() - t_send_data > myRam.pt100_data.sampleRate * 1000)
  {
    if (isConnectedBroker == 1)
    {
      if (myRam.working_status.esp_working_modes == ACTIVE_MODE)
      {
        myRam.mqtt_config_data.isConnectToBroker = 1;
        switch (myRam.flashData_sync.sync_state)
        {
          case START_SEND_FLASH_DATA_TO_SV:
          {           
            //generate json string to send to thingsboard
            StaticJsonDocument<MQTT_MAX_BUFFER> doc_temp_time;
            // ESP_LOGD(TAG, "length data: %d", myRam.data_sync.temp_ptr);
            JsonArray temp_his = doc_temp_time.createNestedArray("temp_his");
            JsonArray time_his = doc_temp_time.createNestedArray("time_his");
            // doc_temp_time["sync_flag"] = 1;
            if (myRam.flashData_sync.total_offline_data_stored > MAX_BUFFER_DATA_POINTS_SEND_TO_SV)
            {
              for (uint16_t i = 0; i < MAX_BUFFER_DATA_POINTS_SEND_TO_SV; i++)
              {
                float tempFlashData;
                String timeFlashData;
                convert_flashData_to_timeTempData(myRam.flashData_sync.ptr_buf_flash_offline_data * 8, tempFlashData, timeFlashData);
                temp_his.add(tempFlashData);
                time_his.add(timeFlashData);
                myRam.flashData_sync.ptr_buf_flash_offline_data ++;
              }
              ESP_LOGD(TAG, "SEND DATA!!!! OVER");
              String output_time_temp;
              serializeJson(doc_temp_time, output_time_temp);
              Serial.println(output_time_temp);
              String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/lastestData";
              send_data_mqtt(mqtt_topic_data, output_time_temp);
              myRam.flashData_sync.total_offline_data_stored -= MAX_BUFFER_DATA_POINTS_SEND_TO_SV;
            }
            else
            {
              for (uint16_t i = 0; i < myRam.flashData_sync.total_offline_data_stored; i++)
              {
                float tempFlashData;
                String timeFlashData;
                convert_flashData_to_timeTempData(myRam.flashData_sync.ptr_buf_flash_offline_data * 8, tempFlashData, timeFlashData);
                temp_his.add(tempFlashData);
                time_his.add(timeFlashData);
                myRam.flashData_sync.ptr_buf_flash_offline_data ++;
                // myRam.data_sync.mqtt_buff_ptr++;
              }
              ESP_LOGD(TAG, "SEND DATA!!!! UNDER");
              String output_time_temp;
              serializeJson(doc_temp_time, output_time_temp);
              Serial.println(output_time_temp);
              String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/lastestData";
              send_data_mqtt(mqtt_topic_data, output_time_temp);
              myRam.flashData_sync.sync_state = CLEAR_FLASH_DATA;
            }
            break;
        }
        default:
          break;
        }

        // switch (myRam.data_sync.sync_state)
        // {
        // case START_SEND_RAM_DATA_TO_SV:
        // {
        //   //generate json string to send to thingsboard
        //   StaticJsonDocument<MQTT_MAX_BUFFER> doc_temp_time;
        //   ESP_LOGD(TAG, "length data: %d", myRam.data_sync.temp_ptr);
        //   JsonArray temp_his = doc_temp_time.createNestedArray("temp_his");
        //   JsonArray time_his = doc_temp_time.createNestedArray("time_his");
        //   // doc_temp_time["sync_flag"] = 1;
        //   if (myRam.data_sync.temp_ptr > MAX_BUFFER_DATA_POINTS_SEND_TO_SV)
        //   {
        //     for (uint16_t i = 0; i < MAX_BUFFER_DATA_POINTS_SEND_TO_SV; i++)
        //     {
        //       temp_his.add(myRam.data_sync.buf_temp[i + myRam.data_sync.mqtt_buff_ptr]);
        //       time_his.add(myRam.data_sync.buf_time[i + myRam.data_sync.mqtt_buff_ptr]);
        //       Serial.print(myRam.data_sync.mqtt_buff_ptr + i);
        //       Serial.print("|");
        //     }
        //     myRam.data_sync.mqtt_buff_ptr += MAX_BUFFER_DATA_POINTS_SEND_TO_SV;
        //     ESP_LOGD(TAG, "SEND DATA!!!! OVER");
        //     String output_time_temp;
        //     serializeJson(doc_temp_time, output_time_temp);
        //     Serial.println(output_time_temp);
        //     String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/lastestData";
        //     // send_data_mqtt(mqtt_topic_data, output_time_temp);
        //     myRam.data_sync.temp_ptr -= MAX_BUFFER_DATA_POINTS_SEND_TO_SV;
        //     myRam.data_sync.sync_state = START_SYNC_RAM_DATA_TO_SV;
        //   }
        //   else
        //   {
        //     for (uint16_t i = 0; i < myRam.data_sync.temp_ptr; i++)
        //     {
        //       temp_his.add(myRam.data_sync.buf_temp[i + myRam.data_sync.mqtt_buff_ptr]);
        //       time_his.add(myRam.data_sync.buf_time[i + myRam.data_sync.mqtt_buff_ptr]);
        //       // myRam.data_sync.mqtt_buff_ptr++;
        //     }
        //     ESP_LOGD(TAG, "SEND DATA!!!! UNDER");
        //     String output_time_temp;
        //     serializeJson(doc_temp_time, output_time_temp);
        //     Serial.println(output_time_temp);
        //     String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/lastestData";
        //     // send_data_mqtt(mqtt_topic_data, output_time_temp);
        //     myRam.data_sync.sync_state = CLEAR_RAM_DATA;
        //   }
        //   break;
        // }
        // default:
        //   break;
        // }
        // // if (myRam.localServer_time.get_time_ok == 1 && (myRam.data_sync.sync_state == FIRST_STARTUP_MEMORY || myRam.data_sync.sync_state == DONE_SYNC_RAM_DATA_TO_SV))
        
        
        if (myRam.ntp_time.get_time_ok == 1 && (myRam.flashData_sync.sync_state == FIRST_STARTUP_FLASH_MEMORY || myRam.flashData_sync.sync_state == DONE_SYNC_FLASH_DATA_TO_SV)) // use this condition for ntp server time
        {
          String output;
          StaticJsonDocument<512> doc;
          doc["mac"] = myRam.wifi_config_data.MAC_Address;
          doc["ip"] = myRam.wifi_config_data.STA_IP;
          doc["SR"] = myRam.pt100_data.sampleRate;
          doc["temp"] = myRam.pt100_data.temp;
          doc["time"] = myRam.ntp_time.ntpDateTimeString; // ntp server time
          // doc["time"] = myRam.localServer_time.localServerDateTimeString; // local server time
          doc["temp_his"][0] = myRam.pt100_data.temp;
          doc["time_his"][0] = myRam.ntp_time.ntpDateTimeString;
          // doc["time_his"][0] = myRam.localServer_time.localServerDateTimeString;
          serializeJson(doc, output);
          Serial.println(output);
          String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/lastestData";
          send_data_mqtt(mqtt_topic_data, output);
        }
      }
      
    }

    t_send_data = millis();
  }
}