#pragma once
#include <Arduino.h>
#include "sensor.h"

#define PIN_OLED_SDA 21
#define PIN_OLED_SCL 22

void initDisplay();
void updateDisplay(const SensorReadings& data, float threshold, bool alarmActive, bool alarmAcked);
void turnOffDisplay();