#define SSID_NAME "M5Cam"
#define SSID_PASSWORD ""
#define URL "http://192.168.4.1/jpg"

#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>

void setup() {
  M5.begin();
  M5.Lcd.setBrightness(255);
  WiFi.begin(SSID_NAME, SSID_PASSWORD);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    // wait for WiFi connection
    delay(1000);
  } else {
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    http.begin(URL);

    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();

    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // HTTP header has been send and Server response header has been handled
    if (httpCode <= 0) {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    } else {
      if (httpCode != HTTP_CODE_OK) {
        Serial.printf("[HTTP] Not OK!\n");
      } else {
        // get lenght of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();
        Serial.printf("[HTTP] size: %d\n", len);

        if (len <= 0) {
          Serial.printf("[HTTP] Unknow content size: %d\n", len);
        } else {
          // create buffer for read
          uint8_t buff[len] = { 0 };

          // get tcp stream
          WiFiClient * stream = http.getStreamPtr();

          // read all data from server
          uint8_t* p = buff;
          int l = len;
          while (http.connected() && (l > 0 || len == -1)) {
            // get available data size
            size_t size = stream->available();

            if (size) {
              int s = ((size > sizeof(buff)) ? sizeof(buff) : size);
              int c = stream->readBytes(p, s);
              p += c;

              Serial.printf("[HTTP] read: %d\n", c);

              if (l > 0) {
                l -= c;
              }
            }
          }

          Serial.println();
          Serial.print("[HTTP] connection closed.\n");

          M5.Lcd.drawJpg(buff, len);
        }
      }
    }

    http.end();
  }
}

