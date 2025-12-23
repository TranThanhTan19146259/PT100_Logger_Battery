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
      if (memcmp(buf[1], PT100_CONFIG_BASE_MQTT_TOPIC, strlen(PT100_CONFIG_BASE_MQTT_TOPIC)) == 0)
      {
        ESP_LOGD(TAG,"correct base config!");
        if (memcmp(buf[2], PT100_CONFIG_TIMESEND_CMD, strlen(PT100_CONFIG_TIMESEND_CMD)) == 0)
        {
          Serial.print("message config: ");
          Serial.write(payload, length);
          Serial.println(); 
          String payload_str;
          for (uint8_t i = 0; i < length; i++)
          {
            payload_str += (char)payload[i];
          }
          Serial.println(payload_str); 
          // parse json string of time send from server 
          StaticJsonDocument<48> doc;
          DeserializationError error = deserializeJson(doc, payload_str);

          if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
          }

          int timeSend = doc["timeSend"]; // 5
          
          ESP_LOGD(TAG,"start configuring time send for esp32 at: %d", timeSend);
          myRam.pt100_data.time_get_data = timeSend;
          // generate response of timesend to publish to server 
          String output;
          String str_timeSend_response;
          str_timeSend_response = "Time send is set to " + String(myRam.pt100_data.time_get_data) + " seconds";
          StaticJsonDocument<80> doc_response_timeSend;
          doc_response_timeSend["response"] = str_timeSend_response;
          serializeJson(doc_response_timeSend, output);
          Serial.println(output);
          send_data_mqtt(PT100_LOGGER_DATA_TOPIC_PUB, output);

          EEPROM.write(0, myRam.pt100_data.time_get_data);
          EEPROM.commit();
        }
        
      }
      if (memcmp(buf[1], PT100_INFO_BASE_MQTT_TOPIC, strlen(PT100_INFO_BASE_MQTT_TOPIC)) == 0)
      {
        ESP_LOGD(TAG,"correct base infor!");
        if (memcmp(buf[2], PT100_STATUS_CMD, strlen(PT100_STATUS_CMD)) == 0)
        {
          Serial.print("message infor: ");
          Serial.write(payload, length);
          Serial.println(); 
          send_data_mqtt(PT100_LOGGER_STATUS_TOPIC_PUB, "pong");
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
      client.subscribe(PT100_LOGGER_TOPIC_SUB);

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

  uint16_t _port = atoi(port);
  ESP_LOGD(TAG,"host: %s\nport: %d\nusername: %s\npassword: %s\n",host, _port, username, password);
  myRam.mqtt_config_data.host = host; 
  myRam.mqtt_config_data.port = _port; 
  myRam.mqtt_config_data.username = username; 
  myRam.mqtt_config_data.password = password; 
  client.setServer(myRam.mqtt_config_data.host.c_str(), myRam.mqtt_config_data.port);
  client.setCallback(callback);
  client.setBufferSize(MQTT_MAX_BUFFER);
  connect_to_broker((char*)myRam.mqtt_config_data.username.c_str(), (char*)myRam.mqtt_config_data.password.c_str());
}

void handle_mqtt()
{
  reconnectMqtt();
  static uint32_t t_send_data = 0;
  static uint32_t t_bk_data = 0;
  static uint8_t state_generate_buf_temp_time_bk;  
  if ((isConnectedBroker == 0 || myRam.wifi_config_data.is_wifi_connected == 0) && myRam.ntp_time.get_time_ok == 1)
  {
    myRam.mqtt_config_data.isConnectToBroker = 0;
    myRam.data_sync.sync_state = START_ASSIGN_DATA_TO_RAM;
  }
  // if (millis() - t_bk_data > 1000)
  // {
  //   // switch (state_generate_buf_temp_time_bk)
  //   // {
  //   // case 0:
  //   //   {
  //   //     myRam.data_sync.buf_temp = new float[BUFFER_TEMP_DATA_BACKUP_SIZE];
  //   //     myRam.data_sync.buf_time = new String[BUFFER_TIME_DATA_BACKUP_SIZE];
  //   //     state_generate_buf_temp_time_bk = 1;
  //   //     break;
  //   //   }
  //   // case 1: 
  //   //   {
  //   //     break;
  //   //   }
  //   // }
    
  //   if ((isConnectedBroker == 0 || myRam.wifi_config_data.is_wifi_connected == 0) && myRam.ntp_time.get_time_ok == 1)
  //   {
  //     myRam.data_sync.sync_state = START_ASSIGN_DATA_TO_RAM;
  //     // if (myRam.data_sync.temp_ptr >= BUFFER_TEMP_DATA_BACKUP_SIZE)
  //     // {
  //     //   myRam.data_sync.temp_ptr = 0;
  //     //   myRam.data_sync.time_ptr = 0;
  //     //   // state_generate_buf_temp_time_bk = 0;
  //     //   delete[] myRam.data_sync.buf_temp; // Deallocate the entire array.
  //     //   myRam.data_sync.buf_temp = nullptr;

  //     //   delete[] myRam.data_sync.buf_time; // Deallocate the entire array.
  //     //   myRam.data_sync.buf_time = nullptr;
  //     //   ESP_LOGD(TAG,"FULLLLLLLL");
  //     //   myRam.data_sync.buf_temp = new float[BUFFER_TEMP_DATA_BACKUP_SIZE];
  //     //   myRam.data_sync.buf_time = new String[BUFFER_TIME_DATA_BACKUP_SIZE];
  //     //   myRam.data_sync.start_sync_data_to_sv = 0;
  //     // }
  //     // else
  //     // {
  //     //   myRam.data_sync.buf_temp[myRam.data_sync.temp_ptr] = myRam.pt100_data.temp;
  //     //   myRam.data_sync.buf_time[myRam.data_sync.time_ptr] = myRam.ntp_time.ntpTimeString;
  //     //   myRam.data_sync.temp_ptr++;
  //     //   myRam.data_sync.time_ptr++;
  //     //   myRam.data_sync.start_sync_data_to_sv = 1;
  //     // }
      
  //     // ESP_LOGD(TAG,"ESP RAM MEMORY: %d totalTempPtr: %d totalTimePtr: %d", ESP.getFreeHeap(), myRam.data_sync.temp_ptr, myRam.data_sync.time_ptr);
  //   }
  //   t_bk_data = millis();
  // }
  

  if (millis() - t_send_data > 1000)
  {
    if (isConnectedBroker == 1)
    {
      if (myRam.working_status.esp_working_modes == ACTIVE_MODE)
      {
        myRam.mqtt_config_data.isConnectToBroker = 1;
        switch (myRam.data_sync.sync_state)
        {
        case START_SEND_RAM_DATA_TO_SV:
        {
          //generate json string to send to thingsboard
          StaticJsonDocument<MQTT_MAX_BUFFER> doc_temp_time;
          ESP_LOGD(TAG, "length data: %d", myRam.data_sync.temp_ptr);
          JsonArray temp_his = doc_temp_time.createNestedArray("temp_his");
          JsonArray time_his = doc_temp_time.createNestedArray("time_his");
          doc_temp_time["sync_flag"] = 1;
          for (uint16_t i = 0; i < myRam.data_sync.temp_ptr; i++)
          {
            temp_his.add(myRam.data_sync.buf_temp[i]);
            time_his.add(myRam.data_sync.buf_time[i]);
          }
          ESP_LOGD(TAG, "SEND DATA!!!!");
          String output_time_temp;
          serializeJson(doc_temp_time, output_time_temp);
          Serial.println(output_time_temp);
          send_data_mqtt(PT100_LOGGER_TB_PUBLISH, output_time_temp);
          myRam.data_sync.sync_state = CLEAR_RAM_DATA;
          break;
        }
        default:
          break;
        }
          
        if (myRam.data_sync.is_remaining_data == 1)
        {

          // myRam.data_sync.sync_state = START_SYNC_RAM_DATA_TO_SV;
          // StaticJsonDocument<MQTT_MAX_BUFFER> doc_temp_time;
          
          // static uint8_t state_sync_data_sv;
          // static uint32_t t_send_data_to_sv;
          // switch (state_sync_data_sv)
          // {
          // case 0:
          // {
          //   t_send_data_to_sv = millis();
          //   ESP_LOGD(TAG, "Start sending data to server");
          //   state_sync_data_sv = 1;
          //   break;
          // }
          // case 1:
          // { 
          //   if (millis() - t_send_data_to_sv > 2000)
          //   {
          //     state_sync_data_sv = 2;
          //     t_send_data_to_sv = millis();
          //   }
          //   break;
          // }
          // case 2:
          // {
          //   ESP_LOGD(TAG, "SEND DATA!!!!");
          //   String output_time_temp;
          //   serializeJson(doc_temp_time, output_time_temp);
          //   Serial.println(output_time_temp);
          //   // send_data_mqtt(PT100_LOGGER_TB_PUBLISH, output_time_temp);
          //   state_sync_data_sv = 3;
          //   break;
          // }
          // case 3:
          // {
          //   ESP_LOGD(TAG, "CLEAR ALL SYNC DATA");
          //   myRam.data_sync.temp_ptr = 0;
          //   myRam.data_sync.time_ptr = 0;
          //   state_generate_buf_temp_time_bk = 0;
          //   delete[] myRam.data_sync.buf_temp; // Deallocate the entire array.
          //   myRam.data_sync.buf_temp = nullptr;

          //   delete[] myRam.data_sync.buf_time; // Deallocate the entire array.
          //   myRam.data_sync.buf_time = nullptr;
          //   myRam.data_sync.start_sync_data_to_sv = 0;
          //   state_sync_data_sv = 0;
          //   break;
          // }
          // default:
          //   break;
          // }
        }
        
        if (myRam.ntp_time.get_time_ok == 1 && (myRam.data_sync.sync_state == FIRST_STARTUP_MEMORY || myRam.data_sync.sync_state == DONE_SYNC_RAM_DATA_TO_SV))
        {
          String output;
          StaticJsonDocument<256> doc;
          doc["temp"] = myRam.pt100_data.temp;
          doc["R"] = myRam.pt100_data.resistor;
          doc["sync_flag"] = 1;
          doc["temp_his"][0] = myRam.pt100_data.temp;
          doc["time_his"][0] = myRam.ntp_time.ntpTimeString;
          serializeJson(doc, output);
          Serial.println(output);
          send_data_mqtt(PT100_LOGGER_TB_PUBLISH, output);
        }
        
        
        // myRam.working_status.esp_working_modes = ENTER_SLEEP_MODE;
      }
      
    }

    t_send_data = millis();
  }
  
  
  
  // static uint32_t t_send_data_to_sv;
  // if (millis() - t_send_data_to_sv > myRam.pt100_data.time_get_data*1000)
  // {
  // t_send_data_to_sv = millis();
  // }
}