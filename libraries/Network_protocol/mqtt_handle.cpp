#include "mqtt_handle.h"
WiFiClient wifiClient;
WiFiClient wifiClientControl;
PubSubClient client(wifiClient);
PubSubClient client_control_topic(wifiClientControl); // new client for control topics


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
    // free(topic);
    // free(payload);
    // free(str);
    // free(token);
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
            // return;
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
        if (memcmp(buf[2], PT100_MQTT_RESPONSE_LASTEST_DATA_TOPIC, strlen(PT100_MQTT_RESPONSE_LASTEST_DATA_TOPIC)) == 0)
        {
          String payload_str;
          for (uint8_t i = 0; i < length; i++)
          {
            payload_str += (char)payload[i];
          }
          Serial.println(payload_str); 
          StaticJsonDocument<512> doc;
          DeserializationError error = deserializeJson(doc, payload_str);
          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
          }

          else
          {
            const char* status = doc["status"];
            if (status && status[0] != '\0')
            {
              // myRam.flashData_sync.response_from_server = 1;
              myRam.flashData_sync.response_from_server_for_lastestData = 1;
            }
          }
        }
        if (memcmp(buf[2], PT100_MQTT_RESPONSE_HISTORY_DATA_TOPIC, strlen(PT100_MQTT_RESPONSE_HISTORY_DATA_TOPIC)) == 0)
        {
          String payload_str;
          for (uint8_t i = 0; i < length; i++)
          {
            payload_str += (char)payload[i];
          }
          Serial.println(payload_str); 
          StaticJsonDocument<512> doc;

          DeserializationError error = deserializeJson(doc, payload_str);

          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
          }
          else
          {
            const char* status = doc["status"];
            if (status && status[0] != '\0')
            {
              myRam.flashData_sync.response_from_server = 1;
            }
          }
        }
        if (memcmp(buf[2], PT100_MQTT_RESPONSE_STATUS, strlen(PT100_MQTT_RESPONSE_STATUS)) == 0)
        {
          String payload_str;
          for (uint8_t i = 0; i < length; i++)
          {
            payload_str += (char)payload[i];
          }
          Serial.println(payload_str); 
          if (payload_str == "pong")
          {
            myRam.mqtt_config_data.mqttConnection = 1;
          }
          
        }
        
      }
      
      
    }



}

void mqtt_status_ack_check()
{
  static uint32_t t_ack_mqtt_sv;
  static uint8_t state_check_mqtt_status;
  switch (state_check_mqtt_status)
  {
  case 0:
    {
      t_ack_mqtt_sv = millis();
      if (myRam.wifi_config_data.wifi_ap_sta == 1 && myRam.wifi_config_data.is_wifi_connected == 1)
      {
        state_check_mqtt_status = 1;
      }
      break;
    }
  case 1:
    {
      if (millis() - t_ack_mqtt_sv > 1000)
      {
        String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/" + PT100_MQTT_STATUS;
        send_data_mqtt(mqtt_topic_data, "ping");
        state_check_mqtt_status = 2;
        t_ack_mqtt_sv = millis();
      }
      break;
    }
  case 2:
    {
      if (myRam.mqtt_config_data.mqttConnection == 1)
      {
        // Serial.println("------------CONECTED TO BROKER-------------------");
        myRam.mqtt_config_data.mqttConnection = 0;
        myRam.mqtt_config_data.isConnectToBroker = 1;
      }
      else
      {
        // Serial.println("------------DISCONECTED TO BROKER----------------");
        // myRam.flashData_sync.sync_state = START_ASSIGN_DATA_TO_FLASH; 

        myRam.mqtt_config_data.isConnectToBroker = 0;
      }
      state_check_mqtt_status = 0;
      break;
    }
  default:
    break;
  } 
  


}


void callback_client_control(char* topic, byte *payload, unsigned int length)
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
            // return;
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
              send_data_mqtt_control_client(mqtt_topic_data, output);
            }
          }
        }
      }
    }
}
bool isConnectedBroker;

void client_control_connect_to_broker(char *usr, char *pass)
{
  static uint8_t disc_to_sv_counter;
  if (!client_control_topic.connected())
  {

    Serial.print("Client Control Attempting MQTT connection...");
    String clientId = "PT100_Logger_control_client_";
    clientId += myRam.wifi_config_data.MAC_Address;

    if (client_control_topic.connect(clientId.c_str(), usr, pass))
    {
      // isConnectedBroker = 1;
      // disc_to_sv_counter = 0;
      Serial.println("client_control connected");
      char buf_sub_topic[100];
      sprintf(buf_sub_topic,"%s/%s/%s",PT100_BASE_MQTT_TOPIC, myRam.mqtt_config_data.devId.c_str(), PT100_MQTT_CONTROL_TOPIC);
      client_control_topic.subscribe(buf_sub_topic);
    }
    else
    {
      // isConnectedBroker = 0;
      // disc_to_sv_counter++;
      // Serial.print("failed, rc=");
      // Serial.print(client_control_topic.state());
      // Serial.println(" try again in 2 seconds");
    }
    //check how many times device disconnect to server
    if (disc_to_sv_counter > MQTT_SERVER_TIMEOUT_COUNT)
    {
      // myRam.working_status.esp_working_modes = ENTER_SLEEP_MODE;
    }
    
  }
}

void connect_to_broker(char *usr, char *pass)
{
  static uint8_t disc_to_sv_counter;
  if (!client.connected())
  {

    Serial.print("Client Attempting MQTT connection...");
    String clientId = "PT100_Logger";
    clientId += myRam.wifi_config_data.MAC_Address;

    if (client.connect(clientId.c_str(), usr, pass))
    {
      isConnectedBroker = 1;
      disc_to_sv_counter = 0;
      Serial.println("connected");
      char buf_sub_topic[100];
      sprintf(buf_sub_topic,"%s/%s/%s",PT100_BASE_MQTT_TOPIC, myRam.mqtt_config_data.devId.c_str(), PT100_MQTT_RESPONSE_STATUS);
      client.subscribe(buf_sub_topic, 0);
      sprintf(buf_sub_topic,"%s/%s/%s",PT100_BASE_MQTT_TOPIC, myRam.mqtt_config_data.devId.c_str(), PT100_MQTT_RESPONSE_HISTORY_DATA_TOPIC);
      client.subscribe(buf_sub_topic, 0);
      // sprintf(buf_sub_topic,"%s/%s/%s",PT100_BASE_MQTT_TOPIC, myRam.mqtt_config_data.devId.c_str(), PT100_MQTT_RESPONSE_LASTEST_DATA_TOPIC);
      // client.subscribe(buf_sub_topic, 0);
    }
    else
    {
      myRam.flashData_sync.sync_state = START_ASSIGN_DATA_TO_FLASH;
      isConnectedBroker = 0;
      disc_to_sv_counter++;
      // Serial.print("failed, rc=");
      // Serial.print(client.state());
      // Serial.println(" try again in 2 seconds");
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
  static unsigned long lastAttempt = 0;
  // already connected → nothing to do
  if (client.connected())
      return;

  // retry every 5 seconds
  if (millis() - lastAttempt < 5000)
  {
    // client.loop();

    return;
  }
  lastAttempt = millis();

  connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
  // if (millis() - t > 5000)
  // {
  //   // client_control_connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
  //   // client_control_topic.loop();
  //   t = millis();
  // }
}

void reconnectMqttClientControl()
{
  static unsigned long t;
  static unsigned long lastAttempt = 0;
  // already connected → nothing to do
  if (client_control_topic.connected())
      return;

  // retry every 5 seconds
  if (millis() - lastAttempt < 5000)
  {
    // client_control_topic.loop();
    return;
  }
  lastAttempt = millis();

  client_control_connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());

  // if (millis() - t > 1000)
  // {
  //   t = millis();
  // }
}

void send_data_mqtt_control_client(String topic, String jsonData)
{
  static unsigned long t;
  reconnectMqttClientControl(); // use when device disconnect to server
  const char *data = jsonData.c_str();
  client_control_topic.publish(topic.c_str(), data); // use to publish data via mqtt
}

void send_data_mqtt(String topic, String jsonData)
{
  static unsigned long t;
  reconnectMqtt(); // use when device disconnect to server
  const char *data = jsonData.c_str();
  client.publish(topic.c_str(), data, false); // use to publish data via mqtt
  // client.endPublish();
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

  // one client connect to broker to send/get lastest data & history data
  client.setServer(myRam.mqtt_config_data.host.c_str(), myRam.mqtt_config_data.port);
  client.setCallback(callback);
  client.setBufferSize(MQTT_MAX_BUFFER);
  // client.setKeepAlive(1);
  // the other connect to broker to send/get lastest data & history data
  // client_control_topic.setServer(myRam.mqtt_config_data.host.c_str(), myRam.mqtt_config_data.port);
  // client_control_topic.setCallback(callback_client_control);
  // connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
  // client_control_connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
  myRam.mqtt_state = FIRST_START_UP;
}

void init_client_control_mqtt()
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

  // one client connect to broker to send/get lastest data & history data
  // client.setServer(myRam.mqtt_config_data.host.c_str(), myRam.mqtt_config_data.port);
  // client.setCallback(callback);
  // client.setBufferSize(MQTT_MAX_BUFFER);

  // the other connect to broker to send/get lastest data & history data
  client_control_topic.setServer(myRam.mqtt_config_data.host.c_str(), myRam.mqtt_config_data.port);
  client_control_topic.setCallback(callback_client_control);
  // client.setBufferSize(MQTT_MAX_BUFFER);
  // connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
  client_control_connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
  // client.setKeepAlive(3);
  myRam.mqtt_state = FIRST_START_UP;
}

void handle_mqtt_client_control()
{
  static uint32_t t_loop_mqtt;
  if (millis() - t_loop_mqtt > 1000)
  {
    /* code */
    // client.loop();
    client_control_topic.loop();
    t_loop_mqtt = millis();
  }
  reconnectMqttClientControl();
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

void mqtt_sync_flash_data_to_sv()
{
  switch (myRam.flashData_sync.sync_state)
  {
    case START_SEND_FLASH_DATA_TO_SV:
    {
      StaticJsonDocument<MQTT_MAX_BUFFER> doc_temp_time;
      // ESP_LOGD(TAG, "length data: %d", myRam.data_sync.temp_ptr);
      JsonArray temp_his = doc_temp_time.createNestedArray("temp_his");
      JsonArray time_his = doc_temp_time.createNestedArray("time_his");
      // doc_temp_time["sync_flag"] = 1;
      if (myRam.flashData_sync.total_offline_data_stored > MAX_BUFFER_DATA_POINTS_SEND_TO_SV)
      {
        // static uint32_t temp_buf_flash_offline = myRam.flashData_sync.ptr_buf_flash_offline_data;
        for (uint16_t i = 0; i < MAX_BUFFER_DATA_POINTS_SEND_TO_SV; i++)
        {
          float tempFlashData;
          String timeFlashData;
          convert_flashData_to_timeTempData(myRam.flashData_sync.flashAddrHead, tempFlashData, timeFlashData);
          temp_his.add(tempFlashData);
          time_his.add(timeFlashData);
          myRam.flashData_sync.flashAddrHead += 8;
          // ESP_LOGD(TAG, "i: %dMAX_BUFFER_DATA_POINTS_SEND_TO_SV %d",i, MAX_BUFFER_DATA_POINTS_SEND_TO_SV);
        }
        Serial.println("SEND DATA!!!! OVER");
        ESP_LOGD(TAG, "SEND DATA!!!! OVER");
        String output_time_temp;
        serializeJson(doc_temp_time, output_time_temp);
        Serial.println(output_time_temp);
        String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/hisData";
        send_data_mqtt(mqtt_topic_data, output_time_temp);
        static uint8_t mini_state_wait_for_response = 0;
        static uint32_t t_wait_for_response = 0;
        if (myRam.flashData_sync.response_from_server == 1)
        {
          myRam.flashData_sync.response_from_server = 0;
        }
        else
        {
          if (myRam.flashData_sync.flashAddrHead != 0)
          {
            myRam.flashData_sync.flashAddrHead -= 8 * MAX_BUFFER_DATA_POINTS_SEND_TO_SV;
          }
        }
        myRam.flashData_sync.sync_state = START_SYNC_FLASH_DATA_TO_SV;
      }
      else
      {
        for (uint16_t i = 0; i < myRam.flashData_sync.total_offline_data_stored; i++)
        {
          float tempFlashData;
          String timeFlashData;
          convert_flashData_to_timeTempData(myRam.flashData_sync.flashAddrHead, tempFlashData, timeFlashData);
          temp_his.add(tempFlashData);
          time_his.add(timeFlashData);
          myRam.flashData_sync.flashAddrHead += 8;
          // myRam.flashData_sync.ptr_buf_flash_offline_data ++;
          // myRam.data_sync.mqtt_buff_ptr++;
        }
        Serial.println("SEND DATA!!!! UNDER");
        ESP_LOGD(TAG, "SEND DATA!!!! UNDER");
        String output_time_temp;
        serializeJson(doc_temp_time, output_time_temp);
        Serial.println(output_time_temp);
        String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/hisData";
        send_data_mqtt(mqtt_topic_data, output_time_temp);
        if (myRam.flashData_sync.response_from_server == 1)
        {
          ESP_LOGD(TAG, "clear data");
          myRam.flashData_sync.sync_state = CLEAR_FLASH_DATA;
          myRam.flashData_sync.response_from_server = 0;
        }
        else
        {
          ESP_LOGD(TAG, "dont recev response from server %d", myRam.flashData_sync.flashAddrHead);
          if (myRam.flashData_sync.flashAddrHead != 0)
          {
            myRam.flashData_sync.flashAddrHead -= (myRam.flashData_sync.total_offline_data_stored * 8);
          }
          
          myRam.flashData_sync.sync_state = START_SYNC_FLASH_DATA_TO_SV;
        }
      }
      break;
    }
  default:
    break;
  }
  // static bool is_data_remaining = 0;
  if (myRam.mqtt_config_data.isConnectToBroker == 1)
  {
    if (myRam.flashData_sync.sync_state == FIRST_STARTUP_FLASH_MEMORY || myRam.flashData_sync.sync_state == DONE_SYNC_FLASH_DATA_TO_SV)
    {
      if (myRam.flashData_sync.total_offline_data_stored != 0)
      {
        ESP_LOGD(TAG, "data remaining");
        myRam.flashData_sync.sync_state = START_SYNC_FLASH_DATA_TO_SV; // sync data after power off and data is still remaining inside flash
      }
      else
      {
      }
    }
    
  }
  
}


void send_lastestData_to_sv(String dateTimeString)
{
  // if (myRam.mqtt_config_data.isConnectToBroker == 1)
  if (isConnectedBroker == 1)
    {
      if (myRam.working_status.esp_working_modes == ACTIVE_MODE)
      {
        // myRam.mqtt_config_data.isConnectToBroker = 1;
        // if (myRam.rtc_time.init_ok == 1 && myRam.mqtt_config_data.mqtt_tick == 1) // use this condition for ntp server time
        // {
          ESP_LOGD(TAG, "--------MQTT SEND------");
          ESP_LOGD(TAG, "Mqtt time: %s", dateTimeString.c_str());
          ESP_LOGD(TAG, "headAddr %d tailAddr %d total %d", myRam.flashData_sync.flashAddrHead, myRam.flashData_sync.flashAddrTail, myRam.flashData_sync.total_offline_data_stored);
          String output;
          StaticJsonDocument<512> doc;
          doc["mac"] = myRam.wifi_config_data.MAC_Address;
          doc["ip"] = myRam.wifi_config_data.STA_IP;
          doc["SR"] = myRam.pt100_data.sampleRate;
          doc["temp"] = myRam.pt100_data.temp;
          doc["RT"] = myRam.pt100_data.reset_times;
          // doc["time"] = myRam.ntp_time.ntpDateTimeString; // ntp server time
          doc["time"] = dateTimeString; // rtc  time
          // doc["time"] = myRam.localServer_time.localServerDateTimeString; // local server time
          doc["temp_his"][0] = myRam.pt100_data.temp;
          doc["time_his"][0] = dateTimeString;
          // doc["time_his"][0] = myRam.localServer_time.localServerDateTimeString;
          serializeJson(doc, output);
          Serial.println(output);
          String mqtt_topic_data = "Indr_PT100/" + myRam.mqtt_config_data.devId + "/lastestData";
          send_data_mqtt(mqtt_topic_data, output);
          myRam.mqtt_config_data.dateTimeLastestBuffer = myRam.rtc_time.rtcDateTimeString;
          myRam.mqtt_state = WAIT_FOR_DATA_RESPONSE;
          myRam.mqtt_config_data.mqtt_tick = 0;
        // }
      }
    }
}

void handle_mqtt()
{
  static uint32_t t_loop_mqtt;
  if (millis() - t_loop_mqtt > 1000)
  {
    /* code */
    if (client.connected()) {
        client.loop();   // fast, non-blocking
    }
    t_loop_mqtt = millis();
  }
  
  reconnectMqtt();
  static uint32_t t_send_data = 0;
  static uint32_t t_bk_data = 0;
  static uint32_t t_wait_for_data_response = 0;
  static uint8_t state_generate_buf_temp_time_bk;  
  // if ((isConnectedBroker == 0 || myRam.wifi_config_data.is_wifi_connected == 0) && myRam.rtc_time.init_ok == 1) // use this condition for ntp server time
  if ((myRam.mqtt_config_data.isConnectToBroker == 0 || myRam.wifi_config_data.is_wifi_connected == 0) && myRam.rtc_time.init_ok == 1) // use this condition for ntp server time
  {
    // myRam.mqtt_config_data.isConnectToBroker = 0;
    // myRam.data_sync.sync_state = START_ASSIGN_DATA_TO_RAM;
    myRam.flashData_sync.sync_state = START_ASSIGN_DATA_TO_FLASH;
  }
  if (myRam.mqtt_config_data.isConnectToBroker)
  {
    mqtt_sync_flash_data_to_sv();
  }
  uint32_t sample_rate;
  sample_rate = myRam.pt100_data.sampleRate * 1000;
    
}