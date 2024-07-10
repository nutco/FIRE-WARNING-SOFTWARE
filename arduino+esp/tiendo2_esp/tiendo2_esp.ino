#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>  //https://github.com/Links2004/arduinoWebSockets

WebSocketsClient webSocket;

const char* ssid = "100k 1 luot";
const char* password = "20082002";
const char* ip_host = "192.168.1.4";
const uint16_t port = 4000;

bool isBtnTurnOnBuzzer = false;
bool isGasSensor1 = false;
bool isGasSensor2 = false;
bool isBuzzerCry = false;
bool btnSystemOn = true;

String dataReceive;

StaticJsonDocument<200> presentstate_jsonData;

void WireWrite(String data) {
  int size = data.length() + 2;  //  \n, \0
  char cData[size];

  snprintf(cData, size, "%s\n", data);
  // Serial.println(cData);

  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(cData);
  Wire.endTransmission();
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        break;
      }
    case WStype_TEXT:
      {
        StaticJsonDocument<200> jsonData;
        deserializeJson(jsonData, payload);
        String command = jsonData["command"];
        Serial.println("kkkkkkkkkkkkkkkkkkkkkkkkkkk");
        Serial.println(command);
        Serial.println("KKKKKKKKKKKKKKKKKKKKKKKKKKK");
        WireWrite(command);
        break;
      }
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", payload);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */

  // Serial.println("ESP8266 Websocket Client");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  webSocket.begin(ip_host, port);
  webSocket.onEvent(webSocketEvent);

  webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();
  send();
}
int count = 0;

void send() {

  static int count = 0;

  Wire.requestFrom(8, 32); /* request & read data of size 13 from slave */
  while (Wire.available()) {
    int c = Wire.read();
    char data = c;
    if (c >= 34 && c <= 127) {  //10 = '\n'
      dataReceive += data;

    } else if (dataReceive != "") {
      if (c == 255) {
        if (count == 0) {
          if (dataReceive == "true") {
            presentstate_jsonData["isBtnTurnOnBuzzer"] = true;

          } else if (dataReceive == "false") {
            presentstate_jsonData["isBtnTurnOnBuzzer"] = false;
          }
        }
        if (count == 1) {
          if (dataReceive == "true") {
            presentstate_jsonData["btnSystemOn"] = true;

          } else {
            presentstate_jsonData["btnSystemOn"] = false;
          }
        }
        if (count == 2) {
          if (dataReceive == "true") {
            presentstate_jsonData["isGasSensor1"] = true;

          } else {
            presentstate_jsonData["isGasSensor1"] = false;
          }
        }
        if (count == 3) {
          if (dataReceive == "true") {
            presentstate_jsonData["isGasSensor2"] = true;

          } else {
            presentstate_jsonData["isGasSensor2"] = false;
          }
          count = 0;
        }
        count++;
        dataReceive = "";
      }

      if (c == 10) {
        if (dataReceive == "true") {
          presentstate_jsonData["isBuzzerCry"] = true;
        } else {
          presentstate_jsonData["isBuzzerCry"] = false;
        }
        String System_info;
        serializeJson(presentstate_jsonData, System_info);
        webSocket.sendTXT(System_info);
        Serial.println("lên server");
        Serial.println(System_info);
        count = 0;

        dataReceive = "";
      }
    }
  }
}
