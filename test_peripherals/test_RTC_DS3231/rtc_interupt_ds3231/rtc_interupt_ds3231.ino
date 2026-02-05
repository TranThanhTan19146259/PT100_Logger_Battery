#include <Wire.h>
#include <RTClib.h>

// Pin definitions
#define I2C_SDA 26
#define I2C_SCL 25
const byte SQWinput = 14;

// RTC object
RTC_DS3231 rtc;

// Interrupt flag
volatile bool timeToRead = false;

// Interrupt service routine
void IRAM_ATTR sqwISR() {
  timeToRead = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32 DS3231 Interrupt Example");
  
  // Initialize I2C with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) delay(10);
  }
  
  // Check if RTC lost power and if so, set the time
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    // Set to date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Configure DS3231 SQW pin for 1Hz output
  // This will generate an interrupt every 1 second
  rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  
  // Setup interrupt pin
  pinMode(SQWinput, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SQWinput), sqwISR, FALLING);
  
  Serial.println("Setup complete. Waiting for interrupts...");
  Serial.println();
}

// Counter for N seconds
int secondCounter = 0;
const int N_SECONDS = 1; // Change this to print every N seconds

void loop() {
  // Check if interrupt flag is set
  if (timeToRead) {
    timeToRead = false;
    secondCounter++;
    
    // Print time every N seconds
    if (secondCounter >= N_SECONDS) {
      secondCounter = 0;
      
      DateTime now = rtc.now();
      
      // Print formatted time
      Serial.print(now.year(), DEC);
      Serial.print('/');
      if (now.month() < 10) Serial.print('0');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      if (now.day() < 10) Serial.print('0');
      Serial.print(now.day(), DEC);
      Serial.print(" ");
      
      if (now.hour() < 10) Serial.print('0');
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      if (now.minute() < 10) Serial.print('0');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      if (now.second() < 10) Serial.print('0');
      Serial.print(now.second(), DEC);
      
      Serial.print(" - Temperature: ");
      Serial.print(rtc.getTemperature());
      Serial.println(" °C");
    }
  }
  
  // You can do other tasks here
  // The interrupt ensures accurate timing
}