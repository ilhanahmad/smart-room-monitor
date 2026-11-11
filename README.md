# Smart Room Monitor

**Technical Test - Shift Engineer / IoT Engineer**  
PT Simple Journey Indonesia  
Ilhan Ahmad Syafa  

---

## 1. Daftar Komponen & Spesifikasi Perangkat

Simulasi dijalankan menggunakan platform **Wokwi Simulator** dengan komponen virtual yang merepresentasikan hardware nyata:

| Komponen | Jumlah | Catatan / Karakteristik Kerja |
| :--- | :---: | :--- |
| **ESP32 DevKit V1 (30-pin)** | 1 | Mikrokontroler utama dengan modul Wi-Fi 2.4 GHz |
| **DHT22 (AM2302)** | 1 | Sensor suhu (-40 s.d. 80 °C) dan kelembapan (0–100% RH) |
| **LDR (Photoresistor)** | 1 | Sensor intensitas cahaya analog |
| **PIR Motion Sensor** | 1 | Deteksi gerakan inframerah pasif (VCC ke VIN/5V, Data 3.3V) |
| **OLED SSD1306 (128x64)** | 1 | Layar I2C (Alamat: `0x3C`) untuk visualisasi lokal |
| **Push Button (ACK)** | 1 | Tombol hening alarm, konfigurasi `INPUT_PULLUP` ke GND |
| **LED Hijau & Merah** | 1 + 1 | Indikator visual kondisi Normal (Hijau) dan Bahaya (Merah) |
| **Resistor 220 Ω** | 2 | Pembatas arus forward bias LED Hijau dan Merah |
| **Piezo Buzzer** | 1 | Peringatan audio frekuensi saat alarm aktif |

---

## 2. Pemetaan Pin (Wiring Table)

| Perangkat / Modul | Pin Perangkat | Pin ESP32 | Mode / Keterangan |
| :--- | :--- | :--- | :--- |
| **DHT22** | SDA / Data | **GPIO4** | Digital I/O (Pull-up 10 kΩ ke 3V3) |
| **LDR** | AO (Analog Out) | **GPIO34** | ADC1_CH6 (Input-only analog) |
| **PIR Sensor** | OUT | **GPIO27** | Digital Input |
| **LED Hijau** | Anoda (+) | **GPIO25** | Digital Output via resistor 220 Ω |
| **LED Merah** | Anoda (+) | **GPIO26** | Digital Output via resistor 220 Ω |
| **Buzzer** | Pin Sinyal (+) | **GPIO33** | Digital Output |
| **Push Button** | Pin | **GPIO14** | Digital Input (`INPUT_PULLUP` internal) |
| **OLED SSD1306** | SDA | **GPIO21** | I2C Data Hardware |
| **OLED SSD1306** | SCL | **GPIO22** | I2C Clock Hardware |
| **Jalur Daya** | VCC DHT/LDR/OLED | **3V3** | Tegangan regulasi 3.3V |
| **Jalur Daya** | VCC PIR | **VIN** | Tegangan 5V dari bus suplai USB |
| **Jalur Ground** | Seluruh GND | **GND** | Jalur ground bersama (*Common Ground*) |

---

## 3. Cara Menjalankan & Membuka Proyek di Wokwi

Struktur file pada simulator Wokwi terdiri atas:
```text
├── sketch.ino        # Loop utama, setup, callback MQTT, & logic deep sleep
├── secrets.h         # Konfigurasi WiFi, Broker MQTT, Port, & Device ID
├── sensor.h / .cpp   # Modul inisialisasi & pembacaan sensor DHT, LDR, PIR
├── alarm.h / .cpp    # State machine logika alarm & push button
├── display.h / .cpp  # Driver layar OLED 
├── libraries.txt     # Daftar library pihak ketiga yang dimuat Wokwi
└── diagram.json      # Konfigurasi peletakan hardware & wiring simulator
```

### Langkah Menjalankan Simulasi:
1. Buka tautan proyek Wokwi: **https://wokwi.com/projects/480193501580132353**.
2. Pastikan seluruh file `.ino`, `.cpp` dan `.h` telah ditambahkan ke dalam projek.
3. Pastikan seluruh library yang dibutuhkan telah diinstall.
4. Klik tombol hijau **Start Simulation** di bagian atas kanvas simulasi.
5. Serial Monitor akan terbuka di bagian bawah kanvas.

---

## 4. Konfigurasi Broker & Pengujian MQTT

Pengujian konektivitas IoT menggunakan broker publik gratis dan web client ([MQTTX Web](https://mqttx.app/web-client)):

- **Broker Host:** `broker.emqx.io`
- **Port:** **`8883`** 
- **Device ID:** `Sesuaikan kebutuhan` 
- **Client ID:** `Sesuaikan kebutuhan`
- **Checklist SSL/TLS & SSL Secure✅** 

### Topik & Format Pesan

#### 1. Telemetri (Publish Tiap 5 Detik)
- **Topik:** `smartroom/room-01/telemetry`
- **Format Payload JSON:**
  ```json
  {
    "device_id": "room-01-testing",
    "temp_c": 31.5,
    "humidity": 65.0,
    "light_raw": 1820,
    "motion": true,
    "alarm": true,
    "uptime_s": 45
  }
  ```

#### 2. Status Perangkat & Last Will (LWT)
- **Topik:** `smartroom/room-01/status`
- **Pesan Online:** `"online"` 
- **Pesan Offline:** `"offline"` 

#### 3. Command (Subscribe)
- **Topik:** `smartroom/room-01/command`
- **Ubah Ambang Suhu (NVS Persistence):**
  ```json
  {"threshold": 32.5}
  ```
- **Pemicu Mode Hemat Daya (Deep Sleep):**
  ```json
  {"sleep_s": 10}
  ```
### Screenshot file .pcap SSL/TLS di Wireshark
<img width="1920" height="1080" alt="Wireshark Pcap TLS" src="https://github.com/user-attachments/assets/94de47bb-6a34-427d-aeb7-3472892a338e" />

---

## 5. Skema Rangkaian

<!-- TEMPLATE: Sertakan gambar skema yang diekspor dari Fritzing, KiCad, atau diagram schematics -->
*(Sisipkan gambar diagram skematik rangkaian di sini atau tautkan file ekspor)*

```markdown
![Skema Rangkaian](docs/schematic_diagram.png)
```

---

## 6. Dokumentasi Rangkaian (Wokwi Simulator)

- **Tautan Simulator Wokwi:** [Wokwi Simulator](https://wokwi.com/projects/480193501580132353)
- **Tangkapan Layar Rangkaian:**
<img width="652" height="672" alt="Screenshot 2026-11-11 111516" src="https://github.com/user-attachments/assets/65edf138-47dc-42b4-801e-35d99c74b0ed" />



## 7. Video Demo

<!-- TEMPLATE: Cantumkan link video demo di Google Drive / YouTube (Unlisted) -->
 **Tautan Video Demo:**
 - [Testing LED, Buzzer & Push Button](https://drive.google.com/file/d/189wxdNXMGUa9cO9flG7Y2CKHakkJsPbu/view?usp=drive_link)
 - [Testing Sensor & OLED](https://drive.google.com/file/d/1hldEvA-KUvR7SWs03KpiyYxqW1UW7rlK/view?usp=drive_link)
 - [Running All Test](https://drive.google.com/file/d/1cAio3-ej7Kkb-AicLLU6ImPLsIG8X40Y/view?usp=drive_link)

**Alur Skenario Demo dalam Video:**
1. **Inisialisasi & Konektivitas:** Menampilkan boot ESP32, koneksi Wi-Fi `Wokwi-GUEST`, handshake TLS port 8883 ke `broker.emqx.io`, dan status `"online"`.
2. **Pembacaan Sensor & OLED:** Nilai DHT22, LDR, dan PIR tampil di OLED serta dipublikasikan tiap 5 detik ke topik telemetri.
3. **Pemicu Alarm & Acknowledge:** Menggeser suhu DHT melewati ambang batas (default 30 derajat). LED Merah berkedip dan Buzzer berbunyi. Tekan push button untuk mematikan buzzer.
4. **Perubahan Ambang Suhu (NVS):** Mengirim perintah `{"threshold": 45}` dari MQTTX Web, status OLED berubah, nilai tersimpan di NVS Flash.
5. **Mode Hemat Daya (Deep Sleep):** Mengirim perintah `{"sleep_s": 20}`. Layar OLED mati total, LED mati, status `"offline"` terkirim. Setelah 10 detik, ESP32 bangun (`[BOOT #2]`) dan memuat kembali ambang suhu `32.5 C` dari NVS.

---

## 8. Catatan Singkat Desain (Design Notes)

### A. Keputusan Desain
1. **Arsitektur Firmware Modular & Non-Blocking:** Seluruh fungsi dibagi ke dalam modul fungsional terpisah (`sensor`, `alarm`, `display`). Pengambilan sampel dan transmisi MQTT menggunakan penjadwalan `millis()` tanpa penggunaan fungsi `delay()` yang memblokir pemrosesan program.
2. **Logika Alarm:** Alarm triggered saat suhu $\ge \text{threshold}$. Saat tombol ditekan, status berubah menjadi *acknowledged* (buzzer mati, LED merah menyala). Alarm baru dapat berbunyi kembali jika suhu turun di bawah ambang batas lalu naik kembali melampaui batas tersebut.
3. **Keamanan Jaringan (TLS/SSL):** Menggunakan `WiFiClientSecure` pada port standar `8883` untuk enkripsi data telemetri di jaringan publik yang dikirim ke broker MQTT agar data penting berbentuk `.json` tidak dapat dilihat secara gamblang pada lapisan jaringan.
4. **Pemisahan Konteks Callback MQTT:** Perintah *sleep* tidak mematikan daya secara langsung di dalam fungsi `onMessage()`, melainkan ditandai via *flag* yang dieksekusi secara tertib di dalam `loop()` utama guna mencegah kegagalan stack TCP/IP MQTT.
5. **Wokwi Simulator:** Keseluruhan sistem disimulasikan secara virtual melalui Wokwi melalui broker MQTT agar lebih praktis dan mempermudah proses pengerjaan hingga review/penilaian.

### B. Masalah yang Ditemui & Solusi Rekayasa
1. **Keterbatasan Simulator Wokwi untuk Flash & Pin EN:**
   - *Kendala:* Menekan tombol stop/restart simulator di browser memformat ulang memori flash virtual Wokwi sehingga partisi NVS ikut terhapus, dan pin header `esp:EN` tidak dapat disimulasikan secara elektrikal.
   - *Solusi:* Validasi NVS dibuktikan melalui siklus *Hardware Deep Sleep Wakeup* (`esp_deep_sleep_start()`) yang mengeksekusi *system reboot* riil dari `setup()` (`[BOOT #2]`), mengosongkan RAM, dan memuat kembali data ambang suhu dari Flash NVS.
2. **Keandalan dan Kestabilan Jaringan WiFi:**
   - *Kendala:* Karena Wokwi Simulator yang digunakan adalah free plan, maka mulai dari proses compile (tergantung load server Wokwi) hingga performa WiFi-Guest (tidak stabil dan terkadang butuh waktu lama untuk connect) memerlukan sedikit waktu dan kesabaran untuk menunggu. 
   - *Solusi:* Jika projek yang dikerjakan cukup berat dan kompleks, disarankan untuk upgrade ke plan berbayar untuk meningkatkan performa.
3. **Keterbatasan Perhitungan Estimasi Konsumsi Arus:**
   - *Kendala:* Tidak bisa menghitung estimasi konsumsi arus riil yang digunakan keseluruhan sistem karena projek dikerjakan secara virtual.
   - *Solusi:* Selain mengerjakan secara virtual, juga merangkai fisik keseluruhan sistem agar dapat mengukur estimasi konsumsi arus riil.

### C. Rencana Peningkatan (Jika Ada Waktu Lebih)
1. **Merangkai Fisik Keseluruhan Sistem:** Dengan merangkai fisik keseluruhan sistem setelah disimulasikan melalui Wokwi, maka keseluruhan sistem akan lebih optimal (termasuk perhitungan riil estimasi konsumsi arus).
