#include "network_handle.h"
#define BUTTON_SIMULATE_NETWORK_CONNECT 4

void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("Lost current wifi");
        // myRam.wifi_config_data.is_wifi_connected = 0;
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        // myRam.wifi_config_data.is_wifi_connected = 1;
        break;
    }
}
void initNetwork()
{
    initWebServer();
    pinMode(BUTTON_SIMULATE_NETWORK_CONNECT, INPUT_PULLUP);
    // WiFi.onEvent(WiFiEvent);
}

void handleNetwork()
{
    static bool now_wifi_status, last_wifi_status;
    if (digitalRead(BUTTON_SIMULATE_NETWORK_CONNECT) == 1)
    {
        if (myRam.wifi_config_data.wifi_ap_sta == 1)
        {
            static uint32_t t_print_debug;
            if (millis() - t_print_debug > 1000)
            {
                ESP_LOGD(TAG, "wifi status: %d", myRam.wifi_config_data.is_wifi_connected);
                t_print_debug = millis();
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                myRam.wifi_config_data.is_wifi_connected = 1;
            }
            else
            {
                static uint32_t t_count_disconnect;
                myRam.wifi_config_data.is_wifi_connected = 0;
                if (millis() - t_count_disconnect > 5000)
                {
                    WiFi.mode(WIFI_STA);
                    WiFi.begin(myRam.wifi_config_data.ssid_sta.c_str(), myRam.wifi_config_data.password_sta.c_str());
                    Serial.print("CONNECTING TO CONNECTED TO ");
                    Serial.println(myRam.wifi_config_data.ssid_sta);
                    static uint32_t t_disc_wifi;
                    ESP_LOGD(TAG, "wifi_disc_count: %d", t_disc_wifi);
                    if (t_disc_wifi > 10)
                    {
                        t_disc_wifi = 10;
                        // myRam.working_status.esp_working_modes = ENTER_SLEEP_MODE;
                    }

                    t_disc_wifi++;
                    t_count_disconnect = millis();
                }

                myRam.wifi_config_data.is_wifi_connected = 0;
            }
            now_wifi_status = myRam.wifi_config_data.is_wifi_connected;
            if (now_wifi_status != last_wifi_status)
            {
                if (now_wifi_status == 1)
                {
                    myRam.wifi_config_data.STA_IP = WiFi.localIP().toString();

                    Serial.print("IP: ");
                    Serial.println(myRam.wifi_config_data.STA_IP);
                    Serial.print("CONNECTED TO ");
                    Serial.println(myRam.wifi_config_data.ssid_sta);
                    String static_ip = myRam.wifi_config_data.STA_IP;
                    char *token = strtok((char *)static_ip.c_str(), "."); // get sta ip
                    int idx = 0;
                    uint16_t ip[4];
                    while (token != NULL)
                    {
                        ip[idx] = atoi(token);
                        token = strtok(NULL, ".");
                        ++idx;
                    }
                    ESP_LOGD(TAG, "static ip: %s", myRam.wifi_config_data.STA_IP.c_str());
                    ESP_LOGD(TAG, "splitted ip: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
                }
                last_wifi_status = now_wifi_status;
            }
        }
    }
    else
    {
        static uint32_t t_count_disconnect;
        myRam.wifi_config_data.is_wifi_connected = 0;
        if (millis() - t_count_disconnect > 5000)
        {
            WiFi.mode(WIFI_STA);
            WiFi.begin("SIMULATE SSID", "SIMULATE SSID");
            Serial.print("CONNECTING TO CONNECTED TO ");
            Serial.println("SIMULATE SSID");
            t_count_disconnect = millis();
        }
        myRam.wifi_config_data.is_wifi_connected = 0;
    }
}
