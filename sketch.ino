#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>      
#include <esp_sleep.h>        
#include "secrets.h"
#include "sensor.h"
#include "alarm.h"
#include "display.h"

// WiFiClientSecure untuk enkripsi TLS pada koneksi MQTT
WiFiClientSecure wifiClient;
PubSubClient mqtt(wifiClient);
Preferences prefs;

// Menyimpan hitungan boot di memori RTC yang bertahan saat Deep Sleep
RTC_DATA_ATTR unsigned long bootCount = 0;

float tempThreshold = 30.0;
unsigned long lastRead = 0;
const unsigned long READ_INTERVAL_MS = 5000; 

unsigned long lastMqttRetry = 0;
const unsigned long MQTT_RETRY_INTERVAL_MS = 5000;
bool firstMqttAttempt = true;

SensorReadings lastData = {25.0, 50.0, 0, false, false};
float currentTemp = 25.0;

int pendingSleepSec = 0;

// Spesifikasi Topik MQTT
String topicTelemetry = String("smartroom/") + DEVICE_ID + "/telemetry";
String topicCommand   = String("smartroom/") + DEVICE_ID + "/command";
String topicStatus    = String("smartroom/") + DEVICE_ID + "/status";

// Mode Hemat Daya (Deep Sleep)
void enterDeepSleep(uint64_t sleepTimeSec) {
  Serial.printf("\n[POWER] Masuk mode Deep Sleep selama %llu detik...\n", sleepTimeSec);

  if (mqtt.connected()) {
    mqtt.publish(topicStatus.c_str(), "offline", true);
    mqtt.loop();
    delay(150);
    mqtt.disconnect();
  }

  turnOffDisplay();
  digitalWrite(PIN_LED_G, LOW);
  digitalWrite(PIN_LED_R, LOW);
  noTone(PIN_BUZZER);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  Serial.flush();
  delay(sleepTimeSec * 1000ULL);

  esp_sleep_enable_timer_wakeup(1000ULL);
  esp_deep_sleep_start();
}

// Callback Subscribed Command
void onMessage(char* topic, byte* payload, unsigned int len) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload, len);

  if (error) {
    Serial.printf("[MQTT] Gagal parse JSON command: %s\n", error.c_str());
    return;
  }

  // Ubah ambang suhu & implementasi NVS/Preferences (Bonus 1)
  if (doc["threshold"].is<float>() || doc["threshold"].is<int>()) {
    tempThreshold = doc["threshold"].as<float>();

    prefs.begin("smartroom", false);
    prefs.putFloat("threshold", tempThreshold);
    prefs.end();

    Serial.printf("[MQTT RX & NVS] Ambang suhu diubah ke: %.1f C\n", tempThreshold);
    updateAlarmLogic(currentTemp, tempThreshold);
    updateDisplay(lastData, tempThreshold, isAlarmActive(), isAlarmAcknowledged());
  }

  // Flagging Deep Sleep untuk dieksekusi di loop()
  if (doc["sleep_s"].is<int>()) {
    int sleepSec = doc["sleep_s"].as<int>();
    if (sleepSec > 0) {
      pendingSleepSec = sleepSec;
    }
  }
}

// Koneksi Wi-Fi
void connectWifi() {
  Serial.printf("[WIFI] Menghubungkan ke %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true); 
  WiFi.begin(WIFI_SSID, WIFI_PASS, 6);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(250);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\n[WIFI] Terhubung! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n[WIFI] Timeout awal! Auto-reconnect berjalan di background.");
  }
}

// Koneksi MQTT over TLS (Port 8883) dengan Last Will & Testament
void connectMqtt() {
  if (WiFi.status() != WL_CONNECTED || mqtt.connected()) return;

  if (!firstMqttAttempt && (millis() - lastMqttRetry < MQTT_RETRY_INTERVAL_MS)) {
    return;
  }
  firstMqttAttempt = false;
  lastMqttRetry = millis();

  Serial.printf("[MQTT TLS] Menghubungkan ke %s:%d...\n", MQTT_HOST, MQTT_PORT);

  randomSeed(micros());
  String clientId = String("esp32-") + DEVICE_ID + "-" + String(random(0xffff), HEX);

  if (mqtt.connect(clientId.c_str(), nullptr, nullptr, topicStatus.c_str(), 1, true, "offline")) {
    Serial.println("[MQTT TLS] Terhubung ke broker dengan enkripsi TLS!");
    mqtt.publish(topicStatus.c_str(), "online", true);
    mqtt.subscribe(topicCommand.c_str());
    Serial.printf("[MQTT TLS] Subscribed ke: %s\n", topicCommand.c_str());
  } else {
    Serial.printf("[MQTT TLS] Gagal terhubung (rc=%d), coba lagi dalam 5 detik.\n", mqtt.state());
  }
}

// Publish telemetri JSON ke smartroom/<device_id>/telemetry tiap 5 detik
void publishTelemetry(const SensorReadings& data) {
  if (!mqtt.connected()) return;

  JsonDocument doc;
  doc["device_id"] = DEVICE_ID;
  doc["temp_c"]    = serialized(String(data.temp, 1));
  doc["humidity"]  = serialized(String(data.hum, 1));
  doc["light_raw"] = data.light;
  doc["motion"]    = data.motion;
  doc["alarm"]     = isAlarmActive();
  doc["uptime_s"]  = millis() / 1000;

  char payload[256];
  serializeJson(doc, payload);

  mqtt.publish(topicTelemetry.c_str(), payload);
  Serial.printf("[MQTT TX] %s -> %s\n", topicTelemetry.c_str(), payload);
}

void setup() {
  Serial.begin(115200);
  bootCount++;

  // Ambil ambang suhu dari NVS
  prefs.begin("smartroom", false);
  tempThreshold = prefs.getFloat("threshold", 30.0);
  prefs.end();
  Serial.printf("\n[BOOT #%lu] Ambang suhu dimuat dari NVS: %.1f C\n", bootCount, tempThreshold);

  initSensor();
  initAlarm();
  initDisplay();

  wifiClient.setInsecure();

  connectWifi();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMessage);

  firstMqttAttempt = true;
  connectMqtt();

  Serial.println("=== SMART ROOM MONITOR ===");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqtt.connected()) {
      connectMqtt();
    } else {
      mqtt.loop();
    }
  }

  // Eksekusi Deep Sleep di luar callback MQTT jika ada perintah masuk
  if (pendingSleepSec > 0) {
    int duration = pendingSleepSec;
    pendingSleepSec = 0;
    enterDeepSleep(duration);
  }

  // Pembacaan & Publish Telemetri
  if (lastRead == 0 || (millis() - lastRead >= READ_INTERVAL_MS)) {
    lastRead = millis();

    lastData = readSensors();
    currentTemp = lastData.temp;

    updateAlarmLogic(currentTemp, tempThreshold);
    updateDisplay(lastData, tempThreshold, isAlarmActive(), isAlarmAcknowledged());

    Serial.printf("[DATA] Suhu: %.1fC (Thres: %.1fC) | Hum: %.1f%% | LDR: %d | PIR: %d | Alarm: %s\n",
                  lastData.temp, tempThreshold, lastData.hum, lastData.light, lastData.motion,
                  isAlarmActive() ? "ON" : (isAlarmAcknowledged() ? "ACK" : "OFF"));

    publishTelemetry(lastData);
  }

  checkAckButton(currentTemp, tempThreshold);
}