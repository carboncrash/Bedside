void dispense() {
  if (!motorJammed) {
    if(!debug) {
      dispenseStartTime = millis();
      while (digitalRead(switchPin)==LOW){ // initial movement to get past switch point
        advanceMotor(millis(), debounce);
        if (millis() > dispenseStartTime + dispenseTimeout) {
          jam();
          break;
        };
      };
      while (digitalRead(switchPin)==HIGH){ // Moving motor to end point
        advanceMotor(millis(), debounce);
        if (millis() > dispenseStartTime + dispenseTimeout) {
          jam();
          break;
        };
      };
      if (!disableMotor) digitalWrite(motorPin, LOW);
    };
    mqtt.publish("/pets/feed/cats", "fed");
    //Spark.publish("doggies_fed","fed");
  }
}

void advanceMotor(long startTime, int runTime) {
  while (millis() < startTime + runTime) if (!disableMotor) digitalWrite(motorPin, HIGH);
}

void jam() {
  if (!disableMotor) digitalWrite(motorPin, LOW);
  mqtt.publish("/pets/feed/cats", "jam");
  motorJammed = true;
}


