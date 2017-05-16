#include <Arduino.h>
#include "AES.h"
#include <ArduinoMD5.h>
#include "helper.h"

struct trackInformation_t {
  String artist;
  String album;
  String title;
};

class Decoder {
private:
  uint8_t* key;
  String keyStr;
  AES aes;

  void unescape(String &str) {
    str.replace("\\\\", "\\");
    str.replace("\\n", "\n");
  };

  bool validateHMAC(uint8_t* buffer, size_t length, uint8_t* hmac) {
    uint8_t* bufferAndKey = (uint8_t*) malloc(length + this->keyStr.length());
    memcpy(bufferAndKey, buffer, length);
    this->keyStr.getBytes(bufferAndKey + length, this->keyStr.length() + 1); // TODO Check this is a bug that needs us to + 1 altough length() should be just fine

    // Serial.println("Key");
    // printHex(bufferAndKey, length + this->keyStr.length());

    uint8_t* hash = (uint8_t*) MD5::make_hash((char*) bufferAndKey, length + this->keyStr.length());

    // Serial.println("Memcmp");
    // Serial.println(memcmp(hash, hmac, 16));

    return memcmp(hash, hmac, 16) == 0;
  };
public:
  Decoder(String key) : keyStr{key} {
    this->key = (uint8_t*) MD5::make_hash(key.c_str());
  };

  trackInformation_t decode(uint8_t* encoded, size_t length) {
    if (length % 16 != 0) {
      Serial.println("Cannot decode buffer because its length isn't a multiple of 16 bit");
      return trackInformation_t{};
    }

    uint8_t* decrypted = decryptAES128CBC(encoded, length);

    uint8_t* hmac = decrypted + length - 16 - 16;

    if (!(this->validateHMAC(decrypted, length - 16 - 16, hmac))) {
      Serial.println("HMAC of received message invalid!");
      return trackInformation_t{"INVALID HMAC", "INVALID HMAC", "INVALID HMAC"};
    }
    else {
      Serial.println("HMAC of received message ok.");
    }

    String decryptedStr = String((char*) decrypted);

    size_t firstNL = decryptedStr.indexOf('\n');
    size_t secondNL = decryptedStr.indexOf('\n', firstNL + 1);
    size_t thirdNL = decryptedStr.indexOf('\n', secondNL + 1);

    if (firstNL == -1 || secondNL == -1 || thirdNL == -1) {
      Serial.println("Did not find three sections in decrypted message.");
      return trackInformation_t{};
    }

    trackInformation_t info;
    info.artist = decryptedStr.substring(0, firstNL);
    info.album = decryptedStr.substring(firstNL + 1, secondNL);
    info.title = decryptedStr.substring(secondNL + 1, thirdNL);
    this->unescape(info.artist);
    this->unescape(info.album);
    this->unescape(info.title);

    return info;
  };

  uint8_t* decryptAES128CBC(uint8_t* buffer, size_t length) {
    //uint8_t* iv = buffer;
    uint8_t* cipher = buffer + 16; // first 16 bytes contain the initialization vector
    uint8_t* decrypted = new uint8_t[length - 16 /* 128 bit block size; first bytes are iv */];
    uint8_t* iv = (uint8_t*) buffer;

    aes.do_aes_decrypt(cipher, length - 16, decrypted, this->key, 128, iv);

    return decrypted;
  };
};
