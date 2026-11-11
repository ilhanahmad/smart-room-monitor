#include "sensor.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT22);
static float lastValidTemp = 25.0;
static float lastValidHum = 50.0;

void initSensor() {
  pinMode(PIN_LDR, INPUT);
  pinMode(PIN_PIR, INPUT);
  dht.begin();
}

SensorReadings readSensors() {
  SensorReadings data;
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Validasi value DHT22 tanpa membuat perangkat hang
  if (isnan(t) || isnan(h)) {
    Serial.println("[WARN] Sensor DHT22 gagal dibaca! Menampilkan nilai valid terakhir.");
    data.temp = lastValidTemp;
    data.hum = lastValidHum;
    data.dhtError = true;
  } else {
    data.temp = t;
    data.hum = h;
    lastValidTemp = t;
    lastValidHum = h;
    data.dhtError = false;
  }

  data.light = analogRead(PIN_LDR);
  data.motion = digitalRead(PIN_PIR);
  return data;
}