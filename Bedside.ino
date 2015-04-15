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
#define lampPin 7
#define switchPin 8
#define EspResetPin 4
#define ledPin 13
boolean lightState = false;
boolean wifiConnected = false;

// SoftwareSerial debugPort(9, 10); // RX, TX
// ESP esp(&Serial, &debugPort, 4);
ESP esp(&Serial, EspResetPin);
MQTT mqtt(&esp);

void setup() {
//  pinMode(switchPin, INPUT_PULLUP);
  pinMode(lampPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(lampPin, LOW);
  digitalWrite(ledPin, LOW);
  Serial.begin(19200);
  //  debugPort.begin(9600);
  //  debugPort.println("Serial started...");
  setupNetworking();
}

void loop(){
  esp.process();
  if (wifiConnected) {}
}
