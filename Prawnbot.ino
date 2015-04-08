/**
 * Prawnbot Three
 * (c) James Wood
 *
 * ESP8266 MQTT Library by Tuan PM <tuanpm@live.com>
 * https://github.com/tuanpmt/esp_mqtt
 *
 */

#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include "secrets.h"

/*
 Create a file named secrets.h in your sketch folder. It should contain the following:
 
 #define wifiname "your wifi network name"
 #define wifipass "your wifi network password"
 #define mqttserver "homeBot" // MQTT server name
 #define mqttport 1883 // usually 1883
 
 */

boolean debug = false;
boolean disableMotor = false;
boolean motorJammed = false;

int motorPin = 5;
int switchPin = 4;
int ledPin = 13;
int dispense(String command);
int dispenseTimeout = 2000;
int debounce = 50;
long dispenseStartTime;

SoftwareSerial debugPort(9, 10); // RX, TX
ESP esp(&Serial, &debugPort, 4);

MQTT mqtt(&esp);
boolean wifiConnected = false;

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");
      mqtt.connect(mqttserver, mqttport, false);
      wifiConnected = true;  
      //or mqtt.connect("host", 1883); /*without security ssl*/
    } 
    else {
      wifiConnected = false;
      mqtt.disconnect();
    }

  }
}

void mqttConnected(void* response) {
  debugPort.println("MQTT Connected");
  mqtt.subscribe("/pets/feed/cats"); //or mqtt.subscribe("topic"); /*with qos = 0*/
  mqtt.publish("/pets/feed/cats/devices", "PrawnBot Online");
}

void mqttDisconnected(void* response) {

}

void mqttData(void* response) {
  RESPONSE res(response);
  debugPort.print("Received: topic=");
  String topic = res.popString();
  debugPort.println(topic);

  debugPort.print("data=");
  String data = res.popString();
  debugPort.println(data);

  if (topic=="/pets/feed/cats" && data=="feed"){
    dispense();
  };
}

void mqttPublished(void* response) {

}

void setup() {

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(motorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(19200);
  debugPort.begin(9600);
  debugPort.println("Serial started...");
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  debugPort.println("ARDUINO: setup mqtt client");
  if(!mqtt.begin("Prawnbot Three", "", "", 120, 1)) {
    debugPort.println("ARDUINO: fail to setup mqtt");
    while(1);
  }

  debugPort.println("ARDUINO: setup mqtt lwt");
  mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

  /*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  debugPort.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect(wifiname, wifipass);

  debugPort.println("ARDUINO: system started");

  if(!debug) {
    debugPort.println("Resetting motor...");
    dispenseStartTime = millis();
    while (digitalRead(switchPin)==HIGH) {
      advanceMotor(millis(), debounce);
      if (millis() > dispenseStartTime + dispenseTimeout) {
        jam();
        break;
      };
    };
    if (!disableMotor) digitalWrite(motorPin, LOW);
    debugPort.println("Motor reset");
  };
}

void loop() {
  esp.process();
  if(wifiConnected) {
  }
  digitalWrite(ledPin, digitalRead(switchPin));
}



