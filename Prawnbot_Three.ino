/**
 * Prawnbot Three
 * (c) James Wood
 */

#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include "secrets.h"

/* Create a file named secrets.h in your sketch folder. It should contain the following:
 
 #define wifiname "your wifi network name"
 #define wifipass "your wifi network password"
 #define mqttserver "homeBot" // MQTT server name
 #define mqttport 1883 // usually 1883
 
 */

bool debug = false;
bool disableMotor = false;
bool motorJammed = false;

int motorPin = 5;
int switchPin = 2;
int ledPin = 13;
int dispense(String command);
int dispenseTimeout = 2000;
int debounce = 50;
long dispenseStartTime;
SoftwareSerial softSerial(2, 3); // RX, TX
ESP esp(&softSerial, &Serial, 4);
MQTT mqtt(&esp);
boolean wifiConnected = false;

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      Serial.println("WIFI CONNECTED");
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

void mqttConnected(void* response)
{
  Serial.println("MQTT Connected");
  mqtt.subscribe("/pets/feed/cats"); //or mqtt.subscribe("topic"); /*with qos = 0*/
  mqtt.publish("/pets/feed/cats", "online");
}

void mqttDisconnected(void* response)
{

}

void mqttData(void* response) {
  RESPONSE res(response);
  Serial.print("Received: topic=");
  String topic = res.popString();
  Serial.println(topic);

  Serial.print("data=");
  String data = res.popString();
  Serial.println(data);

  if (topic="/pets/feed/cats" && data="do"){
    dispense();
  };
}

void mqttPublished(void* response) {

}

void setup() {
  softSerial.begin(19200);
  Serial.begin(19200);
  Serial.println("Serial started...");
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(motorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  if(!debug) {
    while (digitalRead(switchPin)==HIGH) {
      dispenseStartTime = millis();
      advanceMotor(millis(), debounce);
      if (millis() > dispenseStartTime + dispenseTimeout) {
        jam();
        break;
      };
    };
    if (!disableMotor) digitalWrite(motorPin, LOW);
  };

  Serial.println("ARDUINO: setup mqtt client");
  if(!mqtt.begin("Prawnbot Three", "", "", 120, 1)) {
    Serial.println("ARDUINO: fail to setup mqtt");
    while(1);
  }

  Serial.println("ARDUINO: setup mqtt lwt");
  mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

  /*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  Serial.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect(wifiname, wifipass);

  Serial.println("ARDUINO: system started");
}

void loop() {
  esp.process();
  if(wifiConnected) {
  }
  digitalWrite(ledPin, digitalRead(switchPin));
}

