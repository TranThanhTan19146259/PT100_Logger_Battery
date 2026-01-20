#include "hw_handle.h"
TaskHandle_t MqttTask;  
TaskHandle_t GetTimeTask;  
TaskHandle_t Sync_data;
TaskHandle_t Sync_FlashData;
TaskHandle_t MqttLastestDataUpdateTask;

Adafruit_MAX31865 thermo = Adafruit_MAX31865(27, 13, 12, 14); // ESP32 

void control_sleep_mode()
{

  if (myRam.working_status.esp_working_modes == ENTER_SLEEP_MODE)
  {
    static uint8_t sleep_state;
    static uint32_t t_enter_sleep_mode;
    switch (sleep_state)
    {
    case 0:
      {
        t_enter_sleep_mode = millis();
        sleep_state = 1;
        ESP_LOGD(TAG, "Start entering sleep mode !!!!");
        break;
      }
    case 1:
      {
        if (millis() - t_enter_sleep_mode > 10000)
        {
          sleep_state = 2;
        }
        // if (millis() - t_enter_sleep_mode > 8000)
        // {
          // myRam.working_status.esp_working_modes = SEND_LASTWILL_MSG;
        // }
        
      break;
      }
    case 2:
      {
        ESP_LOGD(TAG, "Sleep for %d seconds",myRam.pt100_data.time_get_data);
        myRam.working_status.esp_working_modes = SLEEP;
        // esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, 0); 
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); 
        esp_sleep_enable_timer_wakeup(myRam.pt100_data.time_get_data * 1000000);
        esp_deep_sleep_start();
        sleep_state = 0;
        break;
      }
    default:
      break;
    }
  }
  
}

void factory_reset()
{
  String output, output_wifi, output_pair_key, output_mqtt_server;
  StaticJsonDocument<192> doc_wifi;
  doc_wifi["mode_AP_STA"] = 0;
  JsonArray ssid = doc_wifi.createNestedArray("ssid");
  ssid.add("PT100_LOGGER");
  ssid.add("");
  JsonArray password = doc_wifi.createNestedArray("password");
  password.add("0123456789");
  password.add("");
  serializeJson(doc_wifi, output_wifi);
  writeString_to_spiffs(config_device,output_wifi);
  ESP.restart();
}

void mqtt_handle_task_code(void * parameter)
{
    // Serial.print("Mqtt Task is running on core ");
    if (myRam.wifi_config_data.wifi_ap_sta == 1)
    {
      initMqtt();
    }
    while(1)
    {
        if (myRam.working_status.esp_working_modes != SLEEP)
        {
            handle_mqtt();
        }
    }
}

void time_ntp_task_code(void *parameter)
{
    init_Ntp();
    while(1)
    {
      handle_Ntp();
    }
}

void sync_FlashData_task_code(void *parameter)
{
  init_sync_flashData();
  while (1)
  {
    handle_sync_flashData();
  }
  
}

void sync_data_task_code(void *parameter)
{
  init_sync_data();
  while (1)
  {
    handle_sync_data();
  }
  
}

// void mqtt_lastest_data_update_task_code(void *parameter)
// {
//   while (1)
//   {
//     mqtt_update_data_task();
//   }
  
// }

void init_hw()
{
    EEPROM.begin(10);
    Serial.begin( 115200 ); // baud-rate at 19200
    myRam.pt100_data.time_get_data = EEPROM.read(0); // default time send data set to 1s
    myRam.pt100_data.sampleRate = EEPROM.read(1);
    if (myRam.pt100_data.sampleRate == 0 || myRam.pt100_data.sampleRate > 0xF0)
    {
      /* code */
      myRam.pt100_data.sampleRate = 1; // when the sample of this device did not set, set the sample rate to 1 second for default
    }
    pinMode(CONFIG_BTN, INPUT_PULLUP);
    pinMode(ADC_PIN, INPUT);
    initNetwork();
    myRam.working_status.esp_working_modes = ACTIVE_MODE;

    thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
    // init_Ntp();
    xTaskCreatePinnedToCore(mqtt_handle_task_code,"mqtt",8096,NULL,1,&MqttTask,1);  delay(50);   
    // xTaskCreatePinnedToCore(mqtt_lastest_data_update_task_code,"mqttLastDataUpdate",8096,NULL,1,&MqttLastestDataUpdateTask,1);  delay(50);   
    xTaskCreatePinnedToCore(time_ntp_task_code,"ntp",4096,NULL,1,&GetTimeTask,1);  delay(50);   
    xTaskCreatePinnedToCore(sync_data_task_code,"sync_data",8096,NULL,1,&Sync_data,1);  delay(50);  
    xTaskCreatePinnedToCore(sync_FlashData_task_code,"sync_flash_data",8096,NULL,1,&Sync_FlashData,1);  delay(50);  

    

    // initTimeServerLocal();
    // ESP_LOGD(TAG,"DevId:%s SR: %d", myRam.mqtt_config_data.devId.c_str(), myRam.pt100_data.sampleRate);

    
}

void handle_hw()
{
  control_sleep_mode();
  handleNetwork();
  // handleTimeServerLocal();
  // handle_Ntp();
  
  static uint32_t t_send_data;
  if (millis() - t_send_data > 1000)
  {
    ESP_LOGD(TAG, "adc: %d", analogRead(ADC_PIN));

    size_t total_bytes = SPIFFS.totalBytes();
    size_t used_bytes = SPIFFS.usedBytes();
    size_t free_bytes = total_bytes - used_bytes;
    
    // Serial.printf("Total: %d bytes\n", total_bytes);
    // Serial.printf("Used: %d bytes\n", used_bytes);
    // Serial.printf("Free: %d bytes\n", free_bytes);
    t_send_data = millis();
  }
  

  static uint32_t t_update_data;
  if (millis() - t_update_data > myRam.pt100_data.sampleRate*1000)
  {
    uint16_t rtd = thermo.readRTD();

    // Serial.print("RTD value: "); Serial.println(rtd);
    float ratio = rtd;
    ratio /= 32768;
    // ESP_LOGD(TAG,"btn: %d", digitalRead(CONFIG_BTN));
    // Serial.print("Ratio = "); Serial.println(ratio,8);
    // Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
    // Serial.print("Temperature = "); Serial.println(thermo.temperature(RNOMINAL, RREF));

    // myRam.pt100_data.temp = thermo.temperature(RNOMINAL, RREF);
    myRam.pt100_data.temp = analogRead(ADC_PIN);
    myRam.pt100_data.resistor = RREF*ratio;
    // #ifdef SIMULATE_DATA
    // #else
    // myRam.pt100_data.temp = RREF*ratio;
    // #endif


    // uint8_t fault = thermo.readFault();
    // if (fault) {
    //   Serial.print("Fault 0x"); Serial.println(fault, HEX);
    //   if (fault & MAX31865_FAULT_HIGHTHRESH) {
    //     Serial.println("RTD High Threshold"); 
    //   }
    //   if (fault & MAX31865_FAULT_LOWTHRESH) {
    //     Serial.println("RTD Low Threshold"); 
    //   }
    //   if (fault & MAX31865_FAULT_REFINLOW) {
    //     Serial.println("REFIN- > 0.85 x Bias"); 
    //   }
    //   if (fault & MAX31865_FAULT_REFINHIGH) {
    //     Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    //   }
    //   if (fault & MAX31865_FAULT_RTDINLOW) {
    //     Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    //   }
    //   if (fault & MAX31865_FAULT_OVUV) {
    //     Serial.println("Under/Over voltage"); 
    //   }
    //   thermo.clearFault();
    // }
    // Serial.println();
    t_update_data = millis();
  }
}