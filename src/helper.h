#include <Arduino.h>

const String shorten(String str) {
  if (str.length() <= SCREEN_LINE_SIZE - 2) {
    return str;
  }
  return str.substring(0, 16) + "...";
};
