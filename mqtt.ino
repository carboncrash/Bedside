void setupNetworking(){
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());
  if(!mqtt.begin("Prawnbot", "", "", 120, 1)) {
    while(1);
  }
  mqtt.lwt("/pets/feed/cats/devices/prawnbot", "offline", 0, 0);

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
  mqtt.subscribe("/pets/feed/cats");
  mqtt.publish("/pets/feed/cats/devices/prawnbot", "online");
}

void mqttDisconnected(void* response) {
}

void mqttData(void* response) {
  RESPONSE res(response);
  String topic = res.popString();
  String data = res.popString();
  if (topic=="/pets/feed/cats" && data=="feed") dispense();
}

void mqttPublished(void* response) {
}
