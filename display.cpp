#include "display.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 oled(128, 64, &Wire, -1);
static bool oledReady = false;

void initDisplay() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("[ERROR] OLED gagal diinisialisasi!");
    oledReady = false;
    return;
  }
  oledReady = true;
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("Smart Room Monitor");
  oled.println("Menunggu data...");
  oled.display();
}

void updateDisplay(const SensorReadings& data, float threshold, bool alarmActive, bool alarmAcked) {
  if (!oledReady) return;

  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.printf("THRES: %.1f C %s\n", threshold, data.dhtError ? "[ERR]" : "");
  oled.println("---------------------");
  oled.printf("Suhu : %.1f C\n", data.temp);
  oled.printf("Hum  : %.1f %%\n", data.hum);
  oled.printf("LDR  : %d\n", data.light);
  oled.printf("Gerak: %s\n", data.motion ? "Terdeteksi" : "Aman");

  if (alarmActive) {
    oled.println("ALARM: AKTIF (BAHAYA)");
  } else if (alarmAcked) {
    oled.println("ALARM: ACK (DIMATIKAN)");
  } else {
    oled.println("ALARM: NORMAL");
  }

  oled.display();
}

void turnOffDisplay() {
  if (!oledReady) return;
  oled.clearDisplay();
  oled.display();
  oled.ssd1306_command(SSD1306_DISPLAYOFF);
}