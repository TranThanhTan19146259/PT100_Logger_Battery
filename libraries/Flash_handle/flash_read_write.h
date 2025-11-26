#ifndef FLASH_READ_WRITE_H
#define FLASH_READ_WRITE_H
#include "SPIFFS.h"


void readString_from_spiffs(const char *_filename, String& input_message);


void writeString_to_spiffs(const char *_filename, String &input_message);

#endif