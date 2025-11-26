#include <esp_sleep.h>

// Define the GPIO pin to use for wake up
const int BUTTON_PIN = 13;
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5 
void setup() {
  Serial.begin(115200);
  // Wait for serial connection to establish
  delay(1000); 

  // Print the wake up reason
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: 
      Serial.println("Wakeup caused by external signal on RTC IO"); 
      break;
    default: 
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); 
      break;
  }

  // Enable ext0 wake up on the button pin. 
  // The ESP32 will wake up when the pin is pulled LOW (button pressed).
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); 
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Entering deep sleep...");
  // Wait for a moment to ensure the wake up reason is printed
  delay(1000);
  esp_deep_sleep_start();
}

void loop() {
  // This loop will not be reached in this example, 
  // as the ESP32 will go into deep sleep after setup() completes.
}
