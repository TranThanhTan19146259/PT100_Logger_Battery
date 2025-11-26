#include "flash_read_write.h"
void readString_from_spiffs(const char *_filename, String& input_message)
{
    //debug
    File f = SPIFFS.open(_filename, "r");

    if (!f)
    {
        Serial.println("file open failed");
    }
    else
    {
        //Serial.println("Reading Data from File:" + String(f.size()));
        //Data from file
        for (int i = 0; i < f.size(); i++) //Read upto complete file size
        {
            // *(input_message+i) = ((char)f.read());
            input_message += ((char)f.read());
        }
        // Serial.println(input_message);
        f.close(); //Close file
    }
    //debug
}

void writeString_to_spiffs(const char *_filename, String &input_message)
{
    if (!SPIFFS.exists(_filename))
    {
        File f = SPIFFS.open(_filename, "w");
        f.print(input_message);
        f.close();
    }
    else
    {
        SPIFFS.remove(_filename);
        File f = SPIFFS.open(_filename, "w");
        f.print(input_message);
        f.close();
    }
}