#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

typedef void (*udpCallback_t)(uint8_t* buffer, size_t size, String remoteAddress, uint16_t remotePort);

const uint8_t* REQUEST = (uint8_t*) "request";

class UDPReceiver {
private:
  uint16_t port;
  WiFiUDP udp;
  udpCallback_t callback;
public:
  UDPReceiver(uint16_t port, udpCallback_t callback) {
    this->port = port;
    this->callback = callback;

    this->udp.begin(this->port);
  };

  void check() {
    int bytesAvailable = this->udp.parsePacket();

    if (!bytesAvailable) {
      return;
    }

    uint8_t* buffer = new uint8_t[bytesAvailable];

    this->udp.read(buffer, bytesAvailable);

    this->callback(buffer, bytesAvailable, this->udp.remoteIP().toString(), this->udp.remotePort());
  };

  void sendRequestPacket(uint32_t subnetMask, uint32_t localIP) {
    IPAddress broadcast(~subnetMask | localIP);

    this->udp.beginPacketMulticast(broadcast, 47000, WiFi.localIP());
    this->udp.write(REQUEST, 7);
    this->udp.endPacket();
  };
};
