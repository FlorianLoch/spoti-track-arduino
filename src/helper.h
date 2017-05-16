#ifndef __HELPER_H__
#define __HELPER_H__

#include <Arduino.h>
#include "hardwareConfig.h"

const String shorten(String str) {
  if (str.length() <= SCREEN_LINE_SIZE - 2) {
    return str;
  }
  return str.substring(0, 16) + "...";
};

void printHex(uint8_t* buffer, size_t length) {
  for (size_t i = 0; i < length; i++) {
    // 0000 1111 get printed as F, but we want 0F. Handling needed for
    // all cases where value is below 16 (e.g. 0000 1011, 0000 0001, 0000 0000)
    if (*(buffer + i) < 16) {
      Serial.print("0");
    }

    Serial.print((char) *(buffer + i), HEX);
  }
  Serial.print('\n');
};

#endif
