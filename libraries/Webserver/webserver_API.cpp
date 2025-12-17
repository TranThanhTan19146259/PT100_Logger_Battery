#include "webserver_API.h"

void on_getReq_rs_dev(AsyncWebServerRequest *request)
{
    Serial.println("co thg get ne");

    request->send(200, "text/plain", "OK");
    // request->send(200,"application/json",output);
    delay(2000);
    ESP.restart();
}

void on_getReq_Fw_fs_ver(AsyncWebServerRequest *request)
{
    StaticJsonDocument<80> doc;

    doc["fw_ver"] = FW_VERSION;
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
}
void on_getReq_all_pt100_data(AsyncWebServerRequest *request)
{
    StaticJsonDocument<80> doc;
    Serial.println("so get pt100 data");
    doc["temp"] = myRam.pt100_data.temp;
    doc["R"] = myRam.pt100_data.resistor;
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
}
/*
void on_getReq_add_device_from_mobile_app(AsyncWebServerRequest *request)
{
    // String cmd;
    // DEBUG_SERIAL.println(request->host());
    // DEBUG_SERIAL.println(request->url());
    // DEBUG_SERIAL.println(request->contentType());
    // String data_cmd = "", data_name = "", data_host = "", data_port = "", data_username = "", data_password = "", data_pair_key = "", data_ssid = "", data_password_wifi = "";

    // // Retrieve parameters from the request
    // if (request->hasParam("cmd")) data_cmd = request->getParam("cmd")->value();
    // if (request->hasParam("name")) data_name = request->getParam("name")->value();
    // if (request->hasParam("host")) data_host = request->getParam("host")->value();
    // if (request->hasParam("port")) data_port = request->getParam("port")->value();
    // if (request->hasParam("username")) data_username = request->getParam("username")->value();
    // if (request->hasParam("password")) data_password = request->getParam("password")->value();
    // if (request->hasParam("pair_key")) data_pair_key = request->getParam("pair_key")->value();
    // if (request->hasParam("ssid")) data_ssid = request->getParam("ssid")->value();
    // if (request->hasParam("password_wifi")) data_password_wifi = request->getParam("password_wifi")->value();

    // Log the parameters
    // Serial.println("Received parameters:");
    // Serial.println("cmd: " + cmd);
    // Serial.println("name: " + name);
    // Serial.println("host: " + host);
    // Serial.println("port: " + port);
    // Serial.println("username: " + username);
    // Serial.println("password: " + password);
    // Serial.println("pair_key: " + pair_key);
    // Serial.println("ssid: " + ssid);
    // Serial.println("password_wifi: " + password_wifi);
    // DEBUG_SERIAL.println(request->pathArg(50));
    // String buf;
    // readString_from_spiffs(pair_key_file, buf);
    // ESP_LOGD(ESP_LOGD,"%s",buf.c_str());
    // char *temp = (char*)malloc((len+1)*sizeof(char));
    // if(temp && (len <= 512)) //todo: hardcode
    // {
    //     temp[len] = '\0';
    //     memcpy((void*)temp, (void*)data, len);
    //     ESP_LOGD(TAG, "%s\n", temp);

    //     StaticJsonDocument<512> doc;

    //     DeserializationError error = deserializeJson(doc, (const char*)temp);
    //     free(temp);
    //     if (error) {
    //         ESP_LOGD(TAG,"deserializeJson() failed: ");
    //         ESP_LOGD(ESP_LOGD,"%s",error.c_str());
    //         // sendFailRequest(request);
    //         return;
    //     }
    //     const char* cmd_name = doc["cmd"]["name"]; // "new_device"

    //     JsonObject data = doc["data"];
    //     const char* data_name = data["name"]; // "abc"
    //     const char* data_host = data["host"]; // "123.456.789.123"
    //     const char* data_port = data["port"]; // "1183"
    //     const char* data_username = data["username"]; // "admin"
    //     const char* data_password = data["password"]; // "password"
    //     const char* data_pair_key = data["pair_key"]; // "123456"
    //     const char* data_ssid = data["ssid"]; // "123456"
    //     const char* data_password_wifi = data["password_wifi"]; // "123456"
        String mqtt_host_infor, pair_key_infor, wifi_settings;

        StaticJsonDocument<192> json_raw_wifi_settings;

        json_raw_wifi_settings["mode_AP_STA"] = 1;

        JsonArray ssid = json_raw_wifi_settings.createNestedArray("ssid");
        ssid.add("MOZANIO");
        ssid.add(data_ssid);

        JsonArray password = json_raw_wifi_settings.createNestedArray("password");
        password.add("0123456789");
        password.add(data_password_wifi);
        serializeJson(json_raw_wifi_settings, wifi_settings);
        StaticJsonDocument<128> data_mqtt_host_infor;

        data_mqtt_host_infor["host"] = data_host;
        data_mqtt_host_infor["port"] = data_port;
        data_mqtt_host_infor["username"] = data_username;
        data_mqtt_host_infor["password"] = data_password;

        serializeJson(data_mqtt_host_infor, mqtt_host_infor);

        StaticJsonDocument<48> raw_json_pair_key;

        raw_json_pair_key["pair_key"] = data_pair_key;
        raw_json_pair_key["is_new_device"] = 1;

        serializeJson(raw_json_pair_key, pair_key_infor);
        // serializeJson(doc, output);
        // output = (const char*)temp;
        // Serial.println("add device");
        // serializeJson(doc, output);
        // Serial.println(output);
        String output;
        StaticJsonDocument<48> doc_response;

        doc_response["status"] = "success";

        serializeJson(doc_response, output);
        request->send(200, "application/json", output);
        writeString_to_spiffs(config_mqtt_protocol, mqtt_host_infor);
        writeString_to_spiffs(pair_key_file, pair_key_infor);
        writeString_to_spiffs(config_device, wifi_settings);
        String buf_wifi;
        readString_from_spiffs(config_device, buf_wifi);
        // DEBUG_SERIAL.println(buf_wifi);
        delay(2000);
        ESP.restart();
    // }
    // else
    // {
    //     request->send(405, "text/plain", "Failed: 405");
    // }
}
*/

/*
void on_postReq_change_pair_key(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String buf;
    readString_from_spiffs(pair_key_file, buf);
    ESP_LOGD(ESP_LOGD,"%s",buf.c_str());

    char *temp = (char*)malloc((len+1)*sizeof(char));
    if(temp && (len <= 512)) //todo: hardcode
    {
        temp[len] = '\0';
        memcpy((void*)temp, (void*)data, len);
        ESP_LOGD(TAG, "%s\n", temp);

        StaticJsonDocument<512> doc;

        DeserializationError error = deserializeJson(doc, (const char*)temp);
        free(temp);
        if (error) {
            ESP_LOGD(TAG,"deserializeJson() failed: ");
            ESP_LOGD(ESP_LOGD,"%s",error.c_str());
            // sendFailRequest(request);
            return;
        }
        const char* pair_key = doc["pair_key"];

        myRam.CIT_Mqtt.change_pair_key = 1;
        myRam.CIT_Mqtt.cit_mqtt_server_infor.pairKey = pair_key;
        // String output = "";
        // serializeJson(doc, output);
        Serial.println("change pair key");
        // serializeJson(doc, output);
        // Serial.println(output);
        request->send(200, "text/plain", "OK");

        String pair_key_infor;
        StaticJsonDocument<48> raw_json_pair_key;
        raw_json_pair_key["pair_key"] = pair_key;
        raw_json_pair_key["is_new_device"] = 0;

        serializeJson(raw_json_pair_key, pair_key_infor);
        writeString_to_spiffs(pair_key_file, pair_key_infor);

        // writeString_to_spiffs(pair_key_file,output);
        // ESP.restart();
    }
    else
    {
        request->send(405, "text/plain", "Failed: 405");
    }
}
*/

/*
void on_postReq_add_device(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String buf, pair_key_infor;
    // readString_from_spiffs(pair_key_file, buf);
    // ESP_LOGD(ESP_LOGD,"%s",buf.c_str());
    char *temp = (char*)malloc((len+1)*sizeof(char));
    if(temp && (len <= 512)) //todo: hardcode
    {
        temp[len] = '\0';
        memcpy((void*)temp, (void*)data, len);
        ESP_LOGD(TAG, "%s\n", temp);
        StaticJsonDocument<256> doc;

        DeserializationError error = deserializeJson(doc, (const char*)temp);

        if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
        }

        const char* name = doc["name"]; // "Server 1"
        const char* host = doc["host"]; // "118.69.65.44"
        const char* port = doc["port"]; // "1883"
        const char* username = doc["username"]; // "user1"
        const char* password = doc["password"]; // "12345678"
        uint8_t machine_type = doc["machine_type"]; // 3
        const char* pair_key = doc["pair_key"]; // "258487"

        StaticJsonDocument<128> data_mqtt_host_infor;
        String mqtt_host_infor;
        data_mqtt_host_infor["host"] = host;
        data_mqtt_host_infor["port"] = port;
        data_mqtt_host_infor["username"] = username;
        data_mqtt_host_infor["password"] = password;

        serializeJson(data_mqtt_host_infor, mqtt_host_infor);

        writeString_to_spiffs(config_mqtt_protocol, mqtt_host_infor);

        myRam.Coffee_Machine.machine_type.type = machine_type;
        EEPROM.write(200, myRam.Coffee_Machine.machine_type.type);
        EEPROM.commit();
        StaticJsonDocument<48> raw_json_pair_key;

        raw_json_pair_key["pair_key"] = pair_key;
        raw_json_pair_key["is_new_device"] = 1;

        serializeJson(raw_json_pair_key, pair_key_infor);
        request->send(200, "text/plain", "Done configured");

        writeString_to_spiffs(pair_key_file,pair_key_infor);
        delay(2000);
        ESP.restart();
    }
    else
    {
        request->send(405, "text/plain", "Failed: 405");
    }
}
*/



void on_postReq_setting(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String buf, mqtt_host_infor;
    
    //load data first time to get all data and find data to change
    readString_from_spiffs(config_device, buf);
    StaticJsonDocument<512> doc_default;

    DeserializationError error = deserializeJson(doc_default, buf);
    if (error) {
        // sendFailRequest(request);
        return;
    }
    Serial.println(buf);

    char *temp = (char*)malloc((len+1)*sizeof(char));
    if(temp && (len <= 512)) //todo: hardcode
    {
        temp[len] = '\0';
        memcpy((void*)temp, (void*)data, len);
        ESP_LOGD(TAG, "%s\n", temp);

        StaticJsonDocument<512> doc;

        DeserializationError error = deserializeJson(doc, (const char*)temp);
        free(temp);
        if (error) {
            ESP_LOGD(TAG,"deserializeJson() failed: ");
            ESP_LOGD(ESP_LOGD,"%s",error.c_str());
            // sendFailRequest(request);
            return;
        }
        bool mode_ap_sta = doc[str_setting_mode_ap_sta_param];
        
        const char* ssid = doc[str_setting_ssid_param]; // "0"
        const char* password = doc[str_setting_password_param]; // "1"

        const char* ssid_2 = doc_default[str_setting_ssid_param][!mode_ap_sta]; // "0"
        const char* password_2 = doc_default[str_setting_password_param][!mode_ap_sta]; // "1"
        Serial.println(ssid);
        Serial.println(password);
        Serial.println(ssid_2);
        Serial.println(password_2); 
        

        StaticJsonDocument<512> doc_2;
        // doc_2["flightCode"] = myRam.bus_command_data.flightCode;
        // doc_2["employeeCode"] = myRam.bus_command_data.employeeCode;
        // doc_2["routing"] = myRam.bus_command_data.routing;
        // doc_2["aparture"] = myRam.bus_command_data.aparture;
        // doc_2["departure"] = myRam.bus_command_data.departure;
        // doc.clear();

        // JsonObject doc_0 = doc.createNestedObject();
        doc_2[str_setting_mode_ap_sta_param] = mode_ap_sta;
        doc_2[str_setting_ssid_param][mode_ap_sta] = ssid;
        doc_2[str_setting_password_param][mode_ap_sta] = password;
        doc_2[str_setting_ssid_param][!mode_ap_sta] = ssid_2;
        doc_2[str_setting_password_param][!mode_ap_sta] = password_2;
        String output = "";
        serializeJsonPretty(doc_2, output);
        Serial.println(output);

        request->send(200, "text/plain", "Wifi was configured!");
        
        writeString_to_spiffs(config_device,output);
        ESP.restart();
    }
    else
    {
        request->send(405, "text/plain", "Failed: 405");
    }
 
}

void on_postReq_time_send_pt100_data(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    char *temp = (char*)malloc((len+1)*sizeof(char));
    if(temp && (len <= 512)) //todo: hardcode
    {
        temp[len] = '\0';
        memcpy((void*)temp, (void*)data, len);
        ESP_LOGD(TAG, "%s\n", temp);

        StaticJsonDocument<128> doc;

        DeserializationError error = deserializeJson(doc, (const char*)temp);
        free(temp);
        if (error) {
            ESP_LOGD(TAG,"deserializeJson() failed: ");
            ESP_LOGD(ESP_LOGD,"%s",error.c_str());
            // sendFailRequest(request);
            return;
        }
        uint8_t timeSend = doc["timeSend"]; // 
        ESP_LOGD(TAG, "parsed data timeSend: %d", timeSend);
        EEPROM.write(0, timeSend);
        EEPROM.commit();
        char buf_time_send[80];
        sprintf(buf_time_send, "Time send is set to %d seconds",timeSend);
        request->send(200, "text/plain", buf_time_send);
    }
}

void on_postReq_mqtt_setup(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    char *temp = (char*)malloc((len+1)*sizeof(char));
    if(temp && (len <= 512)) //todo: hardcode
    {
        temp[len] = '\0';
        memcpy((void*)temp, (void*)data, len);
        ESP_LOGD(TAG, "%s\n", temp);

        // StaticJsonDocument<128> doc; 
        String input = temp;
        writeString_to_spiffs(config_mqtt_protocol, input);
        free(temp);
        String output;
        readString_from_spiffs(config_mqtt_protocol, output);
        ESP_LOGD(TAG,"---spiffs data---\n %s",output.c_str());
        request->send(200, "text/plain", "Server set up successfully");
        // DeserializationError error = deserializeJson(doc, (const char*)temp);
        // if (error) {
        //     ESP_LOGD(TAG,"deserializeJson() failed: ");
        //     ESP_LOGD(ESP_LOGD,"%s",error.c_str());
        //     // sendFailRequest(request);
        //     return;
        // }

        // uint8_t timeSend = doc["timeSend"]; // 
        // ESP_LOGD(TAG, "parsed data timeSend: %d", timeSend);
        // EEPROM.write(0, timeSend);
        // EEPROM.commit();

    }
}

/*
void on_postReq_add_device_from_mobile_app(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String buf;
    readString_from_spiffs(pair_key_file, buf);
    ESP_LOGD(ESP_LOGD,"%s",buf.c_str());
    char *temp = (char*)malloc((len+1)*sizeof(char));
    if(temp && (len <= 512)) //todo: hardcode
    {
        temp[len] = '\0';
        memcpy((void*)temp, (void*)data, len);
        ESP_LOGD(TAG, "%s\n", temp);

        StaticJsonDocument<512> doc;

        DeserializationError error = deserializeJson(doc, (const char*)temp);
        free(temp);
        if (error) {
            ESP_LOGD(TAG,"deserializeJson() failed: ");
            ESP_LOGD(ESP_LOGD,"%s",error.c_str());
            // sendFailRequest(request);
            return;
        }
        const char* cmd_name = doc["cmd"]["name"]; // "new_device"

        JsonObject data = doc["data"];
        const char* data_name = data["name"]; // "abc"
        const char* data_host = data["host"]; // "123.456.789.123"
        const char* data_port = data["port"]; // "1183"
        const char* data_username = data["username"]; // "admin"
        const char* data_password = data["password"]; // "password"
        const char* data_pair_key = data["pair_key"]; // "123456"
        const char* data_ssid = data["ssid"]; // "123456"
        const char* data_password_wifi = data["password_wifi"]; // "123456"
        uint8_t data_machine_type = data["machine_type"]; // "123456"
        String mqtt_host_infor, pair_key_infor, wifi_settings;
        
        EEPROM.write(200, data_machine_type);
        EEPROM.commit();

        StaticJsonDocument<192> json_raw_wifi_settings;

        json_raw_wifi_settings["mode_AP_STA"] = 1;

        JsonArray ssid = json_raw_wifi_settings.createNestedArray("ssid");
        ssid.add("MOZANIO");
        ssid.add(data_ssid);

        JsonArray password = json_raw_wifi_settings.createNestedArray("password");
        password.add("0123456789");
        password.add(data_password_wifi);
        serializeJson(json_raw_wifi_settings, wifi_settings);
        StaticJsonDocument<128> data_mqtt_host_infor;

        data_mqtt_host_infor["host"] = data_host;
        data_mqtt_host_infor["port"] = data_port;
        data_mqtt_host_infor["username"] = data_username;
        data_mqtt_host_infor["password"] = data_password;

        serializeJson(data_mqtt_host_infor, mqtt_host_infor);

        StaticJsonDocument<48> raw_json_pair_key;

        raw_json_pair_key["pair_key"] = data_pair_key;
        raw_json_pair_key["is_new_device"] = 1;

        serializeJson(raw_json_pair_key, pair_key_infor);
        // serializeJson(doc, output);
        // output = (const char*)temp;
        // Serial.println("add device");
        // serializeJson(doc, output);
        // Serial.println(output);
        String output;
        StaticJsonDocument<48> doc_response;

        doc_response["status"] = "success";

        serializeJson(doc_response, output);
        request->send(200, "application/json", output);
        writeString_to_spiffs(config_mqtt_protocol, mqtt_host_infor);
        writeString_to_spiffs(pair_key_file, pair_key_infor);
        writeString_to_spiffs(config_device, wifi_settings);
        String buf_wifi;
        readString_from_spiffs(config_device, buf_wifi);
        // DEBUG_SERIAL.println(buf_wifi);
        delay(2000);
        ESP.restart();
    }
    else
    {
        request->send(405, "text/plain", "Failed: 405");
    }
}
*/