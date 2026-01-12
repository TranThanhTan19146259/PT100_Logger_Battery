#include "http_client.h"

HTTPClient http;


int getReqHttp(String serverName, String endpoint, String& responseData)
{
    serverName += endpoint;
    Serial.println(serverName);
    http.begin(serverName.c_str());
    int httpResponseCode = http.GET();
    responseData = http.getString();
    http.end();
    return httpResponseCode;
}


int postReqHttp(String serverName, String& responseData, String jsonData)
{
    http.begin(serverName.c_str());
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    responseData = http.getString();
    }
    else {
    }
    http.end();
    return httpResponseCode;
}

int putReqHttp(String serverName, String endpoint, String& responseData, String jsonData)
{
    serverName += endpoint;
    Serial.println(serverName);
    http.begin(serverName.c_str());
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.PUT(jsonData);
    if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    }
    else {
    }
    http.end();
    return httpResponseCode;
}


int deleteReqHttp(String serverName, String endpoint, String& responseData)
{
    serverName += endpoint;
    http.begin(serverName.c_str());
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.sendRequest("DELETE");
    if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    responseData = http.getString();
    }
    else {
    }
    http.end();
    return httpResponseCode;
}
