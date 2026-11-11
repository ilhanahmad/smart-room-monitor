#pragma once
#include <Arduino.h>

#define PIN_LED_G  25
#define PIN_LED_R  26
#define PIN_BUZZER 33
#define PIN_BTN    14

void initAlarm();
void updateAlarmLogic(float currentTemp, float threshold);
void checkAckButton(float currentTemp, float threshold);
bool isAlarmActive();
bool isAlarmAcknowledged();