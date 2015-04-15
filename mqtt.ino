void setupNetworking(){
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());
  if(!mqtt.begin("JamesBedside", "", "", 120, 1)) {
    while(1);
  }
  mqtt.lwt("upstairs/bedroom/light/james/status", "offline", 0, 0);

  /*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect(wifiname, wifipass);
}

void wifiCb(void* response) {
  uint32_t status;
  RESPONSE res(response);
  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      mqtt.connect(mqttserver, mqttport, false);
      wifiConnected = true;  
    } 
    else {
      wifiConnected = false;
      mqtt.disconnect();
    }
  }
}

void mqttConnected(void* response) {
  mqtt.subscribe("upstairs/bedroom/light/james");
  mqtt.publish("upstairs/bedroom/light/james/status", "online");
}

void mqttDisconnected(void* response) {
}

void mqttData(void* response) {
  RESPONSE res(response);
  String topic = res.popString();
  String data = res.popString();
  if (topic=="upstairs/bedroom/light/james" && data=="ON") {
    digitalWrite(lampPin, HIGH);
    digitalWrite(ledPin, HIGH);
  } else if (topic=="upstairs/bedroom/light/james" && data=="ON") {
    digitalWrite(lampPin, LOW);
    digitalWrite(ledPin, LOW);
  };
}

void mqttPublished(void* response) {
}
