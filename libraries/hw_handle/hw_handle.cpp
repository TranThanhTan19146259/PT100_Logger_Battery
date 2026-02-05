#include "hw_handle.h"
TaskHandle_t MqttTask;  
TaskHandle_t GetTimeTask;  
TaskHandle_t Sync_data;
TaskHandle_t Sync_FlashData;
TaskHandle_t RTC;
TaskHandle_t SaveOfflineDataTask;
TaskHandle_t MqttClientControlTask;
TaskHandle_t MqttAckCheckTask;

hw_timer_t *My_timer = NULL;

Adafruit_MAX31865 thermo = Adafruit_MAX31865(27, 17, 16, 5); // ESP32 
// Adafruit_MAX31865 thermo = Adafruit_MAX31865(27, 13, 12, 14); // ESP32 

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
      // init_client_control_mqtt();
    }
    while(1)
    {
        if (myRam.working_status.esp_working_modes != SLEEP)
        {
          handle_mqtt();
          // handle_mqtt_client_control();
        }
        yield();
    }
}

void mqtt_handle_client_control_task_code(void * parameter)
{
  if (myRam.wifi_config_data.wifi_ap_sta == 1)
  {
    init_client_control_mqtt();
  }
  while (1)
  {
    if (myRam.working_status.esp_working_modes != SLEEP)
    {
      handle_mqtt_client_control();
    }
    yield();
  }
  
}

void time_ntp_task_code(void *parameter)
{
    init_Ntp();
    while(1)
    {
      handle_Ntp();
      yield();
    }
}

void sync_FlashData_task_code(void *parameter)
{
  init_sync_flashData();
  while (1)
  {
    handle_sync_flashData();
    yield();
  }
  
}

void rtc_handle_task_code(void *parameter)
{
  init_rtc_ds3231();
  while (1)
  {
    // Get mutex before modifying myRam
    handle_rtc_ds3231();  // This function updates myRam with RTC time
    // if (xSemaphoreTake(ramDataMutex, portMAX_DELAY) == pdTRUE) {
    //   xSemaphoreGive(ramDataMutex);
    // }
    
    vTaskDelay(pdMS_TO_TICKS(10)); // Add small delay to prevent CPU hogging
  }
  
}

// void save_offline_data_flash_task_code(void *parameter)
// {
//   while (1)
//   {
//     if (myRam.flashData_sync.flash_save_data_tick == 1)
//     {
//       ESP_LOGD(TAG, "FLASH TICK");
//       if ( myRam.flashData_sync.sync_state == START_ASSIGN_DATA_TO_FLASH)
//       {
//           String timeSnapshot = myRam.rtc_time.rtcDateTimeString;
//           save_data_offline_to_flash();
//       }
//       myRam.flashData_sync.flash_save_data_tick = 0;
//     }
//   }
// }

void mqtt_ack_check_status(void *parameter)
{
  while (1)
  {
    mqtt_status_ack_check();
    yield();
  }
  
}

void handle_change_sampleRate()
{
  static uint8_t temp_SR;
  if (temp_SR != myRam.pt100_data.sampleRate)
  {
    temp_SR = myRam.pt100_data.sampleRate;
    // timerStop(My_timer);
    // My_timer = timerBegin(0, MCU_FEQ, true);
    // timerAttachInterrupt(My_timer, &onTimer, true);
    timerAlarmWrite(My_timer, myRam.pt100_data.sampleRate * 1000000, true);
    timerAlarmEnable(My_timer);
    delay(100);
  }
  
}

// void save_flash_task_code(void *parameter)
// {
//   while (1)
//   {
//     // ESP_LOGD(TAG, "--------RUN SAVE FLASH TASK---------- %d", myRam.flashData_sync.flash_save_data_tick);
//     if (myRam.flashData_sync.flash_save_data_tick)
//     {
//       ESP_LOGD(TAG, "--------START ASSIGNING DATA----------");
//       if (myRam.flashData_sync.sync_state == START_ASSIGN_DATA_TO_FLASH)
//       {
//         save_data_offline_to_flash(myRam.rtc_time.rtcDateTimeString);
//       }
//       ESP_LOGD(TAG, "time: %s", myRam.rtc_time.rtcDateTimeString.c_str());
//       myRam.flashData_sync.flash_save_data_tick = 0;
//     }
    
//   }
  
// }



void init_hw()
{
    EEPROM.begin(EEPROM_MAX_SIZE);
    Serial.begin( 115200 ); // baud-rate at 19200
    uint8_t reset_times_count;
    reset_times_count = EEPROM.read(EEPROM_RESET_TIMES);
    myRam.pt100_data.reset_times = reset_times_count;
    reset_times_count++;
    EEPROM.write(EEPROM_RESET_TIMES, reset_times_count);
    myRam.pt100_data.time_get_data = EEPROM.read(0); // default time send data set to 1s
    myRam.pt100_data.sampleRate = EEPROM.read(1);

    // delay(2000);
    if (myRam.pt100_data.sampleRate == 0 || myRam.pt100_data.sampleRate > 0xF0)
    {
      myRam.pt100_data.sampleRate = 1; // when the sample of this device did not set, set the sample rate to 1 second for default
    }
    pinMode(CONFIG_BTN, INPUT_PULLUP);
    pinMode(ADC_PIN, INPUT);
    initNetwork();
    myRam.working_status.esp_working_modes = ACTIVE_MODE;

    thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
    // init_Ntp();
    // xTaskCreatePinnedToCore(sync_FlashData_task_code,"sync_flash_data",8096,NULL,1,&Sync_FlashData,1);  delay(50);  
    // xTaskCreatePinnedToCore(save_offline_data_flash_task_code,"save_offline_data",4096,NULL,1,&SaveOfflineDataTask,1);  delay(50);  
    // delay(2000);
    xTaskCreatePinnedToCore(sync_FlashData_task_code,"sync_flash_data",8096,NULL,1,&Sync_FlashData,1);  
    xTaskCreatePinnedToCore(mqtt_ack_check_status,"mqtt_ack_check",4096,NULL,1,&MqttAckCheckTask,1);   
    xTaskCreatePinnedToCore(mqtt_handle_task_code,"mqtt",10000,NULL,1,&MqttTask,1);   
    xTaskCreatePinnedToCore(time_ntp_task_code,"ntp",4096,NULL,1,&GetTimeTask,1);   
    xTaskCreatePinnedToCore(rtc_handle_task_code,"rtc",4096,NULL,2,&RTC,0);
    xTaskCreatePinnedToCore(mqtt_handle_client_control_task_code,"mqtt_client_control",8096,NULL,1,&MqttClientControlTask,1);   
}

void handle_hw()
{
  // control_sleep_mode();
  handleNetwork();
  
  static uint32_t t_send_data;
  static uint32_t t_update_data;
  uint32_t sample_rate;
  sample_rate = myRam.pt100_data.sampleRate * 1000;
  // if (myRam.pt100_data.sampleRate == 1)
  // {
  //   sample_rate = (myRam.pt100_data.sampleRate * 1000 - OFFSET_SAMPLE_RATE);
  // }
  // else
  // {
  // }
  
  if (millis() - t_update_data > sample_rate)
  {
    uint16_t rtd = thermo.readRTD();

    // Serial.print("RTD value: "); Serial.println(rtd);
    float ratio = rtd;
    ratio /= 32768;
    myRam.pt100_data.temp = analogRead(ADC_PIN);
    myRam.pt100_data.resistor = RREF*ratio;
    t_update_data = millis();
  }
  // yield();
}