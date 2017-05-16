#include <Arduino.h>

class Marquee {
private:
  uint16_t offset = 0;
  uint16_t widthOfText;
  uint16_t screenWidth;
  bool toTheRight = false;
  uint16_t diff;
public:
  Marquee(uint16_t screenWidth) : screenWidth{screenWidth} {};

  void setWidthOfText(uint16_t widthOfText) {
    this->widthOfText = widthOfText;
    diff = widthOfText - screenWidth;
  }

  uint16_t move() {
    if (widthOfText > screenWidth) {
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
    }

    return 0;
  };

  Marquee& reset() {
    toTheRight = false;
    offset = 0;

    return *this;
  };
};
