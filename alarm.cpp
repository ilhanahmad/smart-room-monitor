#include "alarm.h"

static bool alarmActive = false;
static bool alarmAcked = false;

// Variabel untuk non-blocking debounce tombol
static int lastBtnReading = HIGH;
static int stableBtnState = HIGH;
static unsigned long lastDebounceTime = 0;
static const unsigned long DEBOUNCE_MS = 50;

static void applyAlarmOutputs() {
  if (alarmActive) {
    digitalWrite(PIN_LED_G, LOW);
    digitalWrite(PIN_LED_R, HIGH);
    tone(PIN_BUZZER, 1000); 
  } else {
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_R, LOW);
    noTone(PIN_BUZZER);     
  }
}

void initAlarm() {
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
  applyAlarmOutputs();
}

void updateAlarmLogic(float currentTemp, float threshold) {
  if (currentTemp > threshold) {
    if (!alarmAcked) {
      alarmActive = true;
    } else {
      alarmActive = false;
    }
  } else {
    alarmActive = false;
    alarmAcked = false;
  }
  applyAlarmOutputs();
}

void checkAckButton(float currentTemp, float threshold) {
  int reading = digitalRead(PIN_BTN);

  if (reading != lastBtnReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    if (reading != stableBtnState) {
      stableBtnState = reading;
      if (stableBtnState == LOW && alarmActive && currentTemp > threshold) {
        alarmAcked = true;
        alarmActive = false;
        applyAlarmOutputs();
        Serial.println("[ALARM] Tombol ditekan! Alarm dimatikan.");
      }
    }
  }
  lastBtnReading = reading;
}

bool isAlarmActive() {
  return alarmActive;
}

bool isAlarmAcknowledged() {
  return alarmAcked;
}