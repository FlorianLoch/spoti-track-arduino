#include <Arduino.h>

class Marquee {
private:
  uint16_t offset = 0;
  uint8_t fontWidth;
  uint16_t screenWidth;
  bool toTheRight = false;
  uint16_t diff;
  String text;
  bool dontScroll;

  uint16_t _scroll() {
    if (!toTheRight) {
      offset++;
      if (offset == diff) {
        toTheRight = true;
      }
    }
    else {
      offset--;
      if (offset == 0) {
        toTheRight = false;
      }
    }

    return offset;
  };
public:
  Marquee(uint16_t screenWidth, uint8_t fontWidth, String text)
  : screenWidth{screenWidth},
    fontWidth{fontWidth}
  {
    dontScroll = text.length() * fontWidth < screenWidth;

    if (dontScroll) {
      this->text = text;
    }
    else {
      this->text = " " + text + " ";
    }

    diff = this->text.length() * fontWidth - screenWidth;
  };

  Marquee(uint16_t screenWidth, uint8_t fontWidth) {
    String emptyStr("");
    Marquee(screenWidth, fontWidth, emptyStr);
  };

  uint8_t scroll() {
    if (dontScroll) {
      return 0;
    }

    return _scroll() % fontWidth;
  }

  String getVisibleSubstring() {
    uint16_t charsToCutOffAtStart = offset / fontWidth;
    uint16_t lastCharNeeded = charsToCutOffAtStart + (offset + screenWidth) / fontWidth + 1 + 1;

    return text.substring(charsToCutOffAtStart, lastCharNeeded);
  };

  Marquee& reset() {
    toTheRight = false;
    offset = 0;

    return *this;
  };
};
