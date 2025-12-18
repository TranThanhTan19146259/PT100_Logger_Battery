#include "hw_handle.h"

// #include <ModbusRtu.h>
// #include "network_handle.h"
// #include "Ram.h"
// #include "soc/rtc_wdt.h"
// // data array for modbus network sharing
// // uint16_t au16data[16];
// // uint8_t u8state;


// #include <Adafruit_MAX31865.h>

// // Use software SPI: CS, DI, DO, CLK
// // Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13); // Arduino uno
// Adafruit_MAX31865 thermo = Adafruit_MAX31865(27, 13, 12, 14); // ESP32 

// // Adafruit_MAX31865 thermo = Adafruit_MAX31865(4, 5, 6, 7);

// // use hardware SPI, just pass in the CS pin
// // Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// // The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
// #define RREF      430.0
// #define RNOMINAL  100.0
// // #define CONFIG_BTN 13
// #define CONFIG_BTN 26

// void control_sleep_mode()
// {

//   if (myRam.working_status.esp_working_modes == ENTER_SLEEP_MODE)
//   {
//     static uint8_t sleep_state;
//     static uint32_t t_enter_sleep_mode;
//     switch (sleep_state)
//     {
//     case 0:
//       {
//         t_enter_sleep_mode = millis();
//         sleep_state = 1;
//         ESP_LOGD(TAG, "Start entering sleep mode !!!!");
//         break;
//       }
//     case 1:
//       {
//         if (millis() - t_enter_sleep_mode > 10000)
//         {
//           sleep_state = 2;
//         }
//         // if (millis() - t_enter_sleep_mode > 8000)
//         // {
//           // myRam.working_status.esp_working_modes = SEND_LASTWILL_MSG;
//         // }
        
//       break;
//       }
//     case 2:
//       {
//         ESP_LOGD(TAG, "Sleep for %d seconds",myRam.pt100_data.time_get_data);
//         myRam.working_status.esp_working_modes = SLEEP;
//         // esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, 0); 
//         esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); 
//         esp_sleep_enable_timer_wakeup(myRam.pt100_data.time_get_data * 1000000);
//         esp_deep_sleep_start();
//         sleep_state = 0;
//         break;
//       }
//     default:
//       break;
//     }
//   }
  
// }



//reset factory to config 
// void factory_reset()
// {
//   String output, output_wifi, output_pair_key, output_mqtt_server;
//   StaticJsonDocument<192> doc_wifi;

//   doc_wifi["mode_AP_STA"] = 0;

//   JsonArray ssid = doc_wifi.createNestedArray("ssid");
//   ssid.add("PT100_LOGGER");
//   ssid.add("");

//   JsonArray password = doc_wifi.createNestedArray("password");
//   password.add("0123456789");
//   password.add("");
//   // DEBUG_SERIAL.println(output_wifi);
//   serializeJson(doc_wifi, output_wifi);
//   writeString_to_spiffs(config_device,output_wifi);

//   // StaticJsonDocument<256> doc_mqtt_server;
//   // doc_pair_key["host"] = "";
//   // doc_pair_key["port"] = "";
//   // doc_pair_key["username"] = "";
//   // doc_pair_key["password"] = "";
//   // serializeJson(doc_mqtt_server, output_mqtt_server);
//   // writeString_to_spiffs(config_mqtt_protocol,output_mqtt_server);
//   ESP.restart();
// }

void setup() {
  init_hw();
    // EEPROM.begin(10);
    // Serial.begin( 115200 ); // baud-rate at 19200
    // myRam.pt100_data.time_get_data = EEPROM.read(0); // default time send data set to 1s
    // initNetwork();
    // myRam.working_status.esp_working_modes = ACTIVE_MODE;

    // pinMode(CONFIG_BTN, INPUT_PULLUP);

    // if (myRam.wifi_config_data.wifi_ap_sta == 1)
    // {
    //   initMqtt();
    // }
    // thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
    // // delay(5000);
}

void loop() {
  handle_hw();
  // control_sleep_mode();
  // static bool now_wifi_status, last_wifi_status;
  // if (myRam.wifi_config_data.wifi_ap_sta == 1)
  // {
  //     if (WiFi.status() == WL_CONNECTED)
  //     {
  //         myRam.wifi_config_data.is_wifi_connected = 1;
  //         if (myRam.working_status.esp_working_modes != SLEEP)
  //         {
  //           handle_mqtt();
  //         }
  //     }
  //     else
  //     {
  //         static uint32_t t_count_disconnect;

  //         if (millis() - t_count_disconnect > 5000)
  //         {
  //             WiFi.mode(WIFI_STA); 
  //             WiFi.begin(myRam.wifi_config_data.ssid_sta.c_str(), myRam.wifi_config_data.password_sta.c_str());
  //             Serial.print("CONNECTING TO CONNECTED TO ");
  //             Serial.println(myRam.wifi_config_data.ssid_sta);
  //             static uint32_t t_disc_wifi;
  //             ESP_LOGD(TAG,"wifi_disc_count: %d", t_disc_wifi);
  //             if (t_disc_wifi > 10)
  //             {
  //               t_disc_wifi = 10;
  //               myRam.working_status.esp_working_modes = ENTER_SLEEP_MODE;
  //             }
              
  //             t_disc_wifi++;
  //             t_count_disconnect = millis();
  //         }
          
  //         myRam.wifi_config_data.is_wifi_connected = 0;
  //     }
  //     now_wifi_status = myRam.wifi_config_data.is_wifi_connected;
  //     if (now_wifi_status != last_wifi_status )
  //     {
  //         if (now_wifi_status == 1)
  //         {
  //             myRam.wifi_config_data.STA_IP = WiFi.localIP().toString();

  //             Serial.print("IP: ");
  //             Serial.println(myRam.wifi_config_data.STA_IP);
  //             Serial.print("CONNECTED TO ");
  //             Serial.println(myRam.wifi_config_data.ssid_sta);
  //             String static_ip = myRam.wifi_config_data.STA_IP;
  //             char *token = strtok((char*)static_ip.c_str(), "."); // get sta ip
  //             int idx = 0;
  //             uint16_t ip[4];
  //             while(token != NULL){
  //                 ip[idx] = atoi(token);
  //                 token = strtok(NULL, ".");
  //                 ++idx;
  //             }
  //             ESP_LOGD(TAG, "static ip: %s", myRam.wifi_config_data.STA_IP.c_str());
  //             ESP_LOGD(TAG, "splitted ip: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  //             // myRam.Modbus_properties.buf_data_modbus[IP_FIRST_BYTE] = ip[0] | (ip[1] << 8);
  //             // myRam.Modbus_properties.buf_data_modbus[IP_FIRST_BYTE + 1] = ip[2] | (ip[3] << 8);
  //             // myRam.Modbus_properties.read_write_modbus = 1;
  //         }
  //         last_wifi_status = now_wifi_status;
  //     }
  //     static uint8_t config_state;
  //     static uint32_t t_start_configuring;
  //     if (digitalRead(CONFIG_BTN) == 0)
  //     {
  //       myRam.working_status.esp_working_modes = CONFIG_MODE;

  //       switch (config_state)
  //       {
  //       case 0:
  //         {
  //           ESP_LOGD(TAG, "start configuring");    
  //           config_state = 1;
  //           break;
  //         }
  //       case 1:
  //         {
  //           t_start_configuring = millis();
  //           config_state = 2;
  //           break;    
  //         }
  //       case 2:
  //         {
  //           if (millis() - t_start_configuring > 5000)
  //           {
  //             ESP_LOGD(TAG, "CONFIGGGGG");    
  //             factory_reset();
  //             ESP.restart();
  //           }
  //           break;
  //         }
  //       default:
  //         break;
  //       }
  //     }
  //     else
  //     {
  //       config_state = 0;
  //     }
  
  // }
    
  // static uint32_t t_update_data;
  // if (millis() - t_update_data > 1000)
  // {
  //   uint16_t rtd = thermo.readRTD();

  //   // Serial.print("RTD value: "); Serial.println(rtd);
  //   float ratio = rtd;
  //   ratio /= 32768;
  //   // Serial.print("Ratio = "); Serial.println(ratio,8);
  //   // Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  //   // Serial.print("Temperature = "); Serial.println(thermo.temperature(RNOMINAL, RREF));

  //   myRam.pt100_data.temp = thermo.temperature(RNOMINAL, RREF);
  //   myRam.pt100_data.resistor = RREF*ratio;

  //   // uint8_t fault = thermo.readFault();
  //   // if (fault) {
  //   //   Serial.print("Fault 0x"); Serial.println(fault, HEX);
  //   //   if (fault & MAX31865_FAULT_HIGHTHRESH) {
  //   //     Serial.println("RTD High Threshold"); 
  //   //   }
  //   //   if (fault & MAX31865_FAULT_LOWTHRESH) {
  //   //     Serial.println("RTD Low Threshold"); 
  //   //   }
  //   //   if (fault & MAX31865_FAULT_REFINLOW) {
  //   //     Serial.println("REFIN- > 0.85 x Bias"); 
  //   //   }
  //   //   if (fault & MAX31865_FAULT_REFINHIGH) {
  //   //     Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
  //   //   }
  //   //   if (fault & MAX31865_FAULT_RTDINLOW) {
  //   //     Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
  //   //   }
  //   //   if (fault & MAX31865_FAULT_OVUV) {
  //   //     Serial.println("Under/Over voltage"); 
  //   //   }
  //   //   thermo.clearFault();
  //   // }
  //   // Serial.println();
  //   t_update_data = millis();
  // }
  
  

}

