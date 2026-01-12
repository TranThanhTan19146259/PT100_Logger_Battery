#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
#include <HTTPClient.h>

int getReqHttp(String serverName, String endpoint, String& responseData);
int postReqHttp(String serverName, String& responseData, String jsonData);
int putReqHttp(String serverName, String endpoint, String& responseData, String jsonData);
int deleteReqHttp(String serverName, String endpoint, String& responseData);


#endif