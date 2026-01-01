void setup()
{
    Serial.begin(115200);
    pinMode(35, INPUT);
}

void loop()
{
    ESP_LOGD(TAG,"adc: %d", analogRead(2));
    delay(1000);
}