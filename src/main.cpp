#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>
#include "credentials.h"
#include "hardwareConfig.h"
#include "UDPReceiver.h"
#include "helper.h"
#include "Decoder.h"
#include "Marquee.h"

#define FONT u8g2_font_7x14_tf
#define FONT_WIDTH 7
#define FONT_HEIGHT 14

void drawInfo(uint8_t line, Marquee& marquee);

U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock,scl=*/ 14, /* data,sda=*/ 13, /* (NOT USED) cs=*/ U8X8_PIN_NONE, /* dc=*/ 4, /* reset=*/ 5);
//Working with I2C seems not to work, also when soldering the jumpers on the display
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, 5, 4);

const String ssid     = WIFI_SSID;
const char* password = WIFI_KEY;

trackInformation_t currentTrack;
String noInfoYet("No track information received yet... Waiting...");
String empty = "";
Marquee marqueeTitle(SCREEN_WIDTH, FONT_WIDTH, noInfoYet);
Marquee marqueeArtist(SCREEN_WIDTH, FONT_WIDTH, empty);
Marquee marqueeAlbum(SCREEN_WIDTH, FONT_WIDTH, empty);

Decoder decoder("secret");

UDPReceiver udp(47000, [&](uint8_t* buffer, size_t size, String remoteAddress, uint16_t remotePort) -> void {
  Serial.println("Received UDP package from " + remoteAddress + ":" + remotePort);

  currentTrack = decoder.decode(buffer, size);
  marqueeTitle = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.title);
  marqueeArtist = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.artist);
  marqueeAlbum = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.album);

  Serial.println("Playing '" + currentTrack.title + "' from '" + currentTrack.artist + "' on album '" + currentTrack.album + "'");
});

void setup(void) {
  Serial.begin(9600);
  u8g2.begin();

  u8g2.setFont(FONT);

  u8g2.clearBuffer();

  u8g2.drawStr(0, FONT_HEIGHT, "Connecting to WiFi");
  u8g2.drawStr(0, 15 + FONT_HEIGHT, shorten("SSID: " + ssid).c_str());

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

  drawInfo(0, marqueeTitle);
  drawInfo(1, marqueeArtist);
  drawInfo(2, marqueeAlbum);

  u8g2.sendBuffer();

  udp.check();
  yield();
  delay(10);
}

void drawInfo(uint8_t line, Marquee& marquee) {
  uint8_t offset = marquee.scroll();
  const String vs = marquee.getVisibleSubstring();

  uint8_t y = line > 0 ? 20 + line * FONT_HEIGHT : FONT_HEIGHT;

  u8g2.drawStr(-offset, y, vs.c_str());
}
