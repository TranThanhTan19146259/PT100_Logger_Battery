#include <time.h>
#include <WiFi.h>


void setup() {
  Serial.begin(115200);
  WiFi.begin("Indr_PT100_Router", "123456789");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  // GMT+7 offset
  configTime(7 * 3600, 0, "192.168.2.2");
}

void loop() {
  time_t now = time(nullptr);
  Serial.println(ctime(&now));
  delay(1000);
}