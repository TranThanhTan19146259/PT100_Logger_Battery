void setup()
{
    Serial.begin(115200);
    pinMode(2, INPUT);
}

void loop()
{
    ESP_LOGD(TAG,"adc: %d", analogRead(2));
    delay(1000);
}