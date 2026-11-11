#pragma once
#include <Arduino.h>

#define PIN_DHT 4
#define PIN_LDR 34
#define PIN_PIR 27

struct SensorReadings {
  float temp;
  float hum;
  int light;
  bool motion;
  bool dhtError;
};

void initSensor();
SensorReadings readSensors();