# Dew – Smart Environment Monitor
![Espressif](https://img.shields.io/badge/espressif-E7352C.svg?style=for-the-badge&logo=espressif&logoColor=white) ![Adafruit](https://img.shields.io/badge/Adafruit-%23000000.svg?style=for-the-badge&logo=adafruit&logoColor=white) <br>
Dew is a small (cat) temperature and humidity monitor with a distinctive character!!!<br>
The idea behind this project is a small ESP32-based device that monitors the temperature and humidity in a room, displays the data on an OLED screen, and sends it via MQTT to the specified destination. <br> 
<b>Dew is more than just a sensor with a display.</b><br> 
## Hardware 
| Component | Role |
|----------|------|
| ESP32 | Main microcontroller |
| DHT22 | Temperature & humidity sensor |
| SH1106 OLED (128×64) | Display |
| Push Button | User interaction |
## Design 
Everyone loves cats, but cats love to sleep. Dew is a very lazy and sleepy cat, he can fall asleep at any moment. But if you give him a little push, he’s right there, wide awake and ready to do his job. Until the next time it gets too quiet. <br>
<img width="256" height="128" alt="Dew_sleeps" src="https://github.com/user-attachments/assets/790dad6a-5f8b-4728-99d4-f788b2f4e64a"/> <img width="256" height="128" alt="Dew_sleeps(1)" src="https://github.com/user-attachments/assets/bd45cd44-cbf3-49b6-a4a7-6071135be257"/> <img width="256" height="128" alt="Dew_sleeps(2)" src="https://github.com/user-attachments/assets/72d3abe0-3bf5-4383-8c71-6b4c9d1325f4" /> <br>
The app’s magic is in its reactions. Dew is like a Tamagotchi. It doesn’t just show numbers — it reacts to them.<br> 
Too hot? It feels it.<br>
<img width="256" height="128" alt="Dew_hot" src="https://github.com/user-attachments/assets/778dd12c-f9b9-4eb7-9b33-5f43ac5a43ae" /><br>
Stuffy? It knows that, too.<br>
<img width="256" height="128" alt="Dew_stuffy" src="https://github.com/user-attachments/assets/4f46a036-cab6-4478-b3ba-89cc429c86ba" /><br>
Cold or dry? It’ll notice that, too.<br>
<img width="256" height="128" alt="Dew_cold" src="https://github.com/user-attachments/assets/6775024c-3947-4006-ad28-bdc221e3c343"/> <img width="256" height="128" alt="Dew_dry" src="https://github.com/user-attachments/assets/38a3e3a5-0d11-4087-8424-34b5d987b7e9"/><br>
When you first power it on, it introduces itself, walks you through the basics, and casually warns you it might fall asleep at any moment.
Depending on what’s going on around it, Dew changes - and you immediately know something’s wrong, even without looking at the readings. It’s not a sensor with a display, but a little living creature!
## Technical Details
Dew is powered by an ESP32 microcontroller and uses a DHT22 sensor to monitor ambient temperature and humidity. Measurements are displayed on a 128×64 SH1106 OLED display and can be published to an MQTT broker for remote monitoring and home automation integration.
### Features
* Real-time temperature and humidity monitoring
* MQTT integration using Adafruit MQTT libraries
* Automatic WiFi provisioning via WiFiManager
* Interactive OLED interface with custom bitmap graphics
* Single-click sensor refresh
* Double-click display mode switching
* Automatic sleep mode and OLED power management
* Startup introduction
* Sensor data validation and error handling
### MQTT Logic
To reduce network traffic, Dew does not publish every measurement.
New values are sent when:
* Temperature changes by at least 0.5 °C
* Humidity changes by at least 1%
* A minimum reporting interval has elapsed
This ensures efficient communication while still reacting quickly to environmental changes.
### Power Management
OLED displays are susceptible to burn-in. To extend display lifetime, Dew implements a multi-stage power-saving system:
1. Active monitoring mode
2. Sleep animation mode
3. OLED shutdown mode
Any button interaction immediately wakes Dew and restores the display.
### Connectivity
On startup, Dew attempts to connect using stored WiFi credentials. If no known network is available, the device automatically launches a WiFiManager configuration portal, allowing new credentials to be entered without modifying the firmware.
### Software Stack

| Library            | Purpose               |
| ------------------ | --------------------- |
| PinButton          | Button event handling |
| Adafruit SH110X    | OLED display control  |
| DHT Sensor Library | DHT22 communication   |
| WiFiManager        | WiFi provisioning     |
| Adafruit MQTT      | MQTT communication    |
| Wire               | I²C communication     |
## Quick Start
### 1. Clone repository
```bash
git clone https://github.com/your-username/dew.git
cd dew
```
### 2. Install Arduino dependencies
- PinButton
- Adafruit SH110X
- DHT sensor library
- WiFiManager
- Adafruit MQTT Client
- Wire
- Arduino WiFi
### 3. Edit config.h
Fill in your Wi-Fi credentials and MQTT broker details:
```bash
#define WIFI_SSID     "your-network"
#define WIFI_PASS     "your-password"

#define AIO_SERVER    "io.adafruit.com"
#define AIO_PORT      1883
#define IO_USERNAME   "your-username"
#define IO_KEY        "your-key"
```
### Wiring (ESP32)
| Component  | ESP32 Pin  |
| ---------- | ---------- |
| DHT22 DATA | GPIO 33    |
| OLED SDA   | GPIO 21    |
| OLED SCL   | GPIO 22    |
| Button     | GPIO 5     |
| VCC / GND  | 3.3V / GND |
### Flash firmware
Open project in your IDE, select ESP32 Dev Module board and upload code.
You're all done!
