#include <Wire.h>
#include <ArduinoJson.h>
#include <ezButton.h>
#include <math.h>


int btnPhysica1Pin = 3;   //báo cháy
int btnPhysical2Pin = 4;  //tắt báo cháy

int buzzer = 6;

int gasSensor1 = A0;
int gasSensor2 = A1;

int muc_canhbao = 900;

ezButton btnPhysical1(btnPhysica1Pin);   // BAT COI
ezButton btnPhysical2(btnPhysical2Pin);  // TAT HE THONG

String dataReceive = "";

int giatri_gasSensor1 = 0;
int giatri_gasSensor2 = 0;

bool isBtnTurnOnBuzzer = false;
bool isGasSensor1 = false;
bool isGasSensor2 = false;
bool isBuzzerCry = false;
bool btnSystemOn = true;

StaticJsonDocument<200> previousJsonData;
String presentstate_data;

void setup() {

  pinMode(buzzer, OUTPUT);
  pinMode(gasSensor1, INPUT);
  pinMode(gasSensor2, INPUT);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(115200);

  previousJsonData["isBtnTurnOnBuzzer"] = isBtnTurnOnBuzzer;
  previousJsonData["btnSystemOn"] = btnSystemOn;
  previousJsonData["isGasSensor1"] = isGasSensor1;
  previousJsonData["isGasSensor2"] = isGasSensor2;
  previousJsonData["isBuzzerCry"] = isBuzzerCry;
}

void TurnOnBuzzer() {
  tone(buzzer, 4000);
  isBuzzerCry = true;
}

void TurnOffBuzzer() {
  noTone(buzzer);
  isBuzzerCry = false;
}
void TurnOnWhere() {
  if (isGasSensor1 || isGasSensor2) {
    TurnOnBuzzer();
  }
  if (!isGasSensor1 && !isGasSensor2 && !isBtnTurnOnBuzzer) {
    TurnOffBuzzer();
  }
}
void TurnOff() {
  TurnOffBuzzer();
}

void TurnOn() {
  TurnOnBuzzer();
}

void loop() {
  btnPhysical1.loop();
  btnPhysical2.loop();
  update();
  // Serial.println("ss1");
  // Serial.println(analogRead(gasSensor1));
  // Serial.println("ss2");
  // Serial.println(analogRead(gasSensor2));
  // delay(1000);
}

void update() {
  giatri_gasSensor1 = analogRead(gasSensor1);
  giatri_gasSensor2 = analogRead(gasSensor2);

  if (giatri_gasSensor1 > muc_canhbao) {

    isGasSensor1 = true;
  } else {
    isGasSensor1 = false;
  }
  if (giatri_gasSensor2 > muc_canhbao) {
    isGasSensor2 = true;
  } else {
    isGasSensor2 = false;
  }

  if (!isGasSensor1 && !isGasSensor2 && !isBtnTurnOnBuzzer) {
    TurnOff();
  }

  if (btnSystemOn == true) {
    if (btnPhysical1.isPressed()) {
      isBtnTurnOnBuzzer = true;
      TurnOn();
    } else {
      TurnOnWhere();
    }

    if (btnPhysical2.isPressed()) {
      btnSystemOn = false;
      TurnOff();
    }
  }
}

void receiveEvent(int howMany) {
  while (0 < Wire.available()) {
    char readData = Wire.read();
    Serial.println(readData);

    if (readData != '\n') {
      dataReceive += readData;

    } else {
      if (dataReceive == "SWITCH") {
        Serial.println(dataReceive);
        // btnSystemOn == true;
        if (isBtnTurnOnBuzzer == true) {
          isBtnTurnOnBuzzer = false;
          TurnOff();
        } else {
          isBtnTurnOnBuzzer = true;
          TurnOn();
        }
        // Serial.println("ẹuindj");
      }
      Serial.println(isBtnTurnOnBuzzer);
      dataReceive = "";
    }
  }
}

void requestEvent() {
  send();
}

void sendESP(String content) {

  if (content.length() <= 32 && content != "") {
    Wire.write(content.c_str(), content.length());
    Serial.println(content);

  } else {
    Serial.println("LIMIT 32 BYTE OR STRING NULL, EMPTY");
    Serial.println(content.length());
  }
}

String ConvertToBool(bool b) {
  if (b) {
    return "true";
  } else {
    return "false";
  }
}

char rst = 255;
char line = 10;
void send() {
  if (
    previousJsonData["isBtnTurnOnBuzzer"] != isBtnTurnOnBuzzer || previousJsonData["btnSystemOn"] != btnSystemOn || previousJsonData["isGasSensor1"] != isGasSensor1 || previousJsonData["isGasSensor2"] != isGasSensor2 || previousJsonData["isBuzzerCry"] != isBuzzerCry) {
    previousJsonData["isBtnTurnOnBuzzer"] = isBtnTurnOnBuzzer;
    previousJsonData["btnSystemOn"] = btnSystemOn;
    previousJsonData["isGasSensor1"] = isGasSensor1;
    previousJsonData["isGasSensor2"] = isGasSensor2;
    previousJsonData["isBuzzerCry"] = isBuzzerCry;


    String isBtnTurnOnBuzzerTemp = ConvertToBool(isBtnTurnOnBuzzer);
    String btnSystemOnTemp = ConvertToBool(btnSystemOn);
    String isGasSensor1Temp = ConvertToBool(isGasSensor1);
    String isGasSensor2Temp = ConvertToBool(isGasSensor2);
    String isBuzzerCryTemp = ConvertToBool(isBuzzerCry);

    String s = isBtnTurnOnBuzzerTemp + rst + btnSystemOnTemp + rst + isGasSensor1Temp + rst + isGasSensor2Temp + rst + isBuzzerCryTemp + line;
    Serial.println(s);

    sendESP(s);
  }
}