#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

// Define your custom I2C pins
#define I2C_SDA 26
#define I2C_SCL 25

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  // Check if RTC lost power
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();
  
  Serial.print(now.year());
  Serial.print('/');
  Serial.print(now.month());
  Serial.print('/');
  Serial.print(now.day());
  Serial.print(" ");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.println(now.second());
  
  delay(1000);
}