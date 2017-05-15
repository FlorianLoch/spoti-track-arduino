#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>
#include "credentials.h"
#include "hardwareConfig.h"
#include "UDPReceiver.h"
#include "helper.h"
#include "Decoder.h"

U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock,scl=*/ 14, /* data,sda=*/ 13, /* (NOT USED) cs=*/ U8X8_PIN_NONE, /* dc=*/ 4, /* reset=*/ 5);
//Working with I2C seems not to work, also when soldering the jumpers on the display
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, 5, 4);

const String ssid     = WIFI_SSID;
const char* password = WIFI_KEY;

Decoder decoder("secret");

UDPReceiver udp(47000, [&](uint8_t* buffer, size_t size, String remoteAddress, uint16_t remotePort) -> void {
  Serial.println("Received UDP package from " + remoteAddress + ":" + remotePort);

  trackInformation_t info = decoder.decode(buffer, size);

  Serial.println("Playing '" + info.title + "' from '" + info.artist + "' on album '" + info.album + "'");
});

void setup(void) {
  Serial.begin(9600);
  u8g2.begin();

  // text display tests
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont12_tf);
  u8g2.drawStr(0, 10, "Connecting to WiFi:");
  u8g2.drawStr(0, 28, shorten("SSID: " + ssid).c_str());

  u8g2.sendBuffer();

  WiFi.begin(ssid.c_str(), password);

  uint8_t widthOfDot = u8g2.getStrWidth(".");
  uint8_t dotOffset = -widthOfDot;
  while (WiFi.status() != WL_CONNECTED) {
    delay(80);
    Serial.print(".");
    u8g2.drawStr(dotOffset = dotOffset + widthOfDot, 38, ".");
    u8g2.sendBuffer();
    yield();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  udp.sendRequestPacket(WiFi.subnetMask(), WiFi.localIP());
}

void loop(void) {
  u8g2.clearBuffer();
  u8g2.sendBuffer();

  udp.check();

  delay(10);

  yield();
}
