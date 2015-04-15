/** Prawnbot (c) James Wood - ESP8266 MQTT Library by Tuan PM <tuanpm@live.com> https://github.com/tuanpmt/esp_mqtt & http://tuanpm.net/post/espduino
Create a file named secrets.h in your sketch folder. It should contain the following:
#define wifiname "your wifi network name"
#define wifipass "your wifi network password"
#define mqttserver "homeBot" // MQTT server name
#define mqttport 1883 // usually 1883
*/

#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include "secrets.h"
#define debounce 50
#define dispenseTimeout 2000
#define motorPin 5
#define switchPin 4
#define EspResetPin 11
#define ledPin 13
boolean debug = false;
boolean disableMotor = false;
boolean motorJammed = false;
boolean wifiConnected = false;

// SoftwareSerial debugPort(9, 10); // RX, TX
// ESP esp(&Serial, &debugPort, 4);
ESP esp(&Serial, EspResetPin);
MQTT mqtt(&esp);



void setup() {
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(motorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(motorPin, LOW);
  digitalWrite(ledPin, !digitalRead(switchPin));
  Serial.begin(19200);
  //  debugPort.begin(9600);
  //  debugPort.println("Serial started...");
  resetMotor();
  setupNetworking();
}

void loop() {
  digitalWrite(motorPin, LOW);
  esp.process();
  if (wifiConnected) {}
  digitalWrite(ledPin, !digitalRead(switchPin));
}
