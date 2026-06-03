#include <PinButton.h>
#include <Wire.h>           
#include <Adafruit_SH110X.h>
#include <DHT.h>
#include <WiFi.h>
#include "Adafruit_MQTT_Client.h"
#include <WiFiManager.h>
#include "config.h"
#include "bitmaps.h"

PinButton myButton(5);
const unsigned long INTERVAL_MQTT_PUBLISH = 25UL * 60UL * 1000UL, INTERVAL_SLEEP_ON = 2UL * 60UL * 1000UL, INTERVAL_SLEEP_ON_SCREEN_OFF = 3UL * 60UL * 1000UL, BURN_TIME = 60UL * 2000UL;      
const unsigned long MIN_MQTT_INTERVAL = 3UL * 60UL * 1000UL;

Adafruit_SH1106G display(128, 64, &Wire, -1);
DHT dht(33, DHT22);
WiFiClient wifiClient;
Adafruit_MQTT_Client mqtt(&wifiClient, AIO_SERVER, AIO_PORT, IO_USERNAME, IO_KEY);
Adafruit_MQTT_Publish feedTemp = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish feedHum = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/humidity");

const float TEMP_LOW_MAX = 15.0;   
const float TEMP_HIGH_MIN = 25.0;   
const float HUM_LOW_MAX = 33.0;   
const float HUM_HIGH_MIN = 66.0;  
float last_sent_temp = -999.0;
float last_sent_hum = -999.0;

struct IntroFrame {
  const uint8_t* bmp;
  const char* lines[4];
};

float temperature = 0, humidity = 0;
bool should_read = false;
unsigned long last_update = 0, last_disp_on = 0, sleep_time = 0;
bool screen_mode = 0; // 0 = Temp, 1 = Hum
bool isFirstReadDone = false, isDisplayOn = true, IsDewSleeping = false;
const uint8_t* current_sleep_bmp = nullptr; 
bool introPlayed = false;
unsigned long last_mqtt_time = 0;

void update_display() {
	if (!isDisplayOn) return;
  display.clearDisplay();

  const uint8_t* bmp = select_bitmap();
  if (bmp) {
    display.drawBitmap(0, 0, bmp, 128, 64, SH110X_WHITE);
  }

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);

  if (!isFirstReadDone) {
		display.setCursor(12, 24);
    display.print("Scanning"); 
  } else if (screen_mode == 0) {
		display.setTextSize(1);
		display.setCursor(103, 8);
		display.print("TEMP");
    char buf[10];
    snprintf(buf, sizeof(buf), "%.1fC", temperature);
    int16_t x1, y1;
    uint16_t w, h;
    display.setTextSize(2);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(128 - w, 24);
    display.print(buf);
  } else if (screen_mode == 1) {
		display.setTextSize(1);
		display.setCursor(110, 8);
		display.print("HUM");
    char buf[10];
    snprintf(buf, sizeof(buf), "%.1f%%", humidity);
    int16_t x1, y1;
    uint16_t w, h;
    display.setTextSize(2);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(128 - w, 24);
    display.print(buf);
  }
  	display.display();
}

void wifi_connect() {
  if (WiFi.status() == WL_CONNECTED) return;
  show_status("Connecting WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    show_status("Connected WiFi");
    return;
  }

  Serial.println("Starting WiFiManager...");
  show_status("No WiFi.  Starting  manager");
  WiFiManager wm;
  wm.autoConnect("ESP32-Setup");
  show_status("Connected WiFi");
}

void mqtt_connect(bool silent = false) {
  if (mqtt.connected()) return;

  if (!silent) show_status("Connecting Server");
  Serial.print("Connecting to MQTT... ");

  int8_t ret;
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    mqtt.disconnect();
    delay(3000); 
    retries--;
    if (retries == 0) {
      Serial.println("MQTT connection failed. Moving on.");
      if (!silent) {
        show_status("Server Failed");
        delay(1000);
      }
      return; 
    }
  }
  Serial.println("MQTT Connected!");
  if (!silent) {
    show_status("Connected Server");
    delay(500);
    update_display();
  }
}
  
void off_display() {
  if (isDisplayOn) {
    display.clearDisplay();
    display.oled_command(SH110X_DISPLAYOFF);
    display.display();
    isDisplayOn = false;
    Serial.println("Screen is off");
  }
}

void wake_display() {
  last_disp_on = millis();
  IsDewSleeping = false;
  if (!isDisplayOn) {
    display.oled_command(SH110X_DISPLAYON);
    isDisplayOn = true;
		Serial.println("Screen is on");
  }
}

void sleep_display() {
  current_sleep_bmp = showSleepBitmap(); 
  
  if (current_sleep_bmp) {
    display.clearDisplay();
    display.drawBitmap(0, 0, current_sleep_bmp, 128, 64, SH110X_WHITE);
    display.display();
  }
  
  IsDewSleeping = true;
  sleep_time = millis();
  Serial.println("Dew is sleeping");
}

bool wait_for_button() {
  while (true) { 
    myButton.update(); 
    if (myButton.isSingleClick() || myButton.isDoubleClick()) {
      return true; 
    }   
    delay(5);
  }
}

void play_intro() {
  IntroFrame intro[] = {
    {bmp_intro_1, {"Hi, my name", "is Dew!", "", ""}},
    {bmp_intro_4, {"Tap to", "refresh...", "double tap", "to switch."}},
    {bmp_intro_3, {"Be careful.", "Dew can fall", "asleep any", "time... zZ"}},
  };

  int frames = sizeof(intro) / sizeof(intro[0]);
  for (int i = 0; i < frames; i++) {
    display.clearDisplay();
    if (intro[i].bmp) {
      display.drawBitmap(0, 0, intro[i].bmp, 128, 64, SH110X_WHITE);
    }
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);

    int lineCount = 0;
    for (int j = 0; j < 4; j++) {
      if (intro[i].lines[j] != "") {
        lineCount++;
      }
    }
    int lineHeight = 10;
    int totalTextHeight = lineCount * lineHeight;
    int startY = (64 - totalTextHeight) / 2;
    int currentLine = 0;
    for (int j = 0; j < 4; j++) {
      if (intro[i].lines[j] != "") {
        display.setCursor(58, startY + currentLine * lineHeight);
        display.print(intro[i].lines[j]);
        currentLine++;
      }
    }
    display.display();
    wait_for_button();
  }
  display.clearDisplay();
  display.display();
}

void show_status(const char* msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);
  int x = (128 - w) / 2;
  int y = (64 - h) / 2;    
  display.setCursor(x, y);
  display.print(msg);
  display.display();
}

void mqtt_publish() {
  unsigned long now = millis();
  bool time_to_send = (now - last_mqtt_time >= MIN_MQTT_INTERVAL);
  bool temp_changed = abs(temperature - last_sent_temp) >= 0.5;
  bool hum_changed = abs(humidity - last_sent_hum) >= 1.0;

  if (!isnan(temperature) && !isnan(humidity) && temperature < 50.0 && humidity <= 100.0) {
    if (time_to_send || temp_changed || hum_changed) {
      if (feedTemp.publish(temperature)) {
        last_sent_temp = temperature;
      }
      delay(500);
      if (feedHum.publish(humidity)) {
        last_sent_hum = humidity;
      }
      last_mqtt_time = now;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(22, 21);
  randomSeed(esp_random());
  display.begin(0x3C, true);
  display.clearDisplay();
  display.display();
  if (!introPlayed) {
  play_intro();
  introPlayed = true;
  }
  last_disp_on = millis();
  last_update = millis() - INTERVAL_MQTT_PUBLISH + 2500;
  update_display(); 
  dht.begin();
	wifi_connect();
  mqtt_connect();
}

void loop() {
  myButton.update();

  if (myButton.isSingleClick()) {
    should_read = true;
    wake_display();
    update_display();
  }

  if (myButton.isDoubleClick()) {
    wake_display();
    screen_mode = !screen_mode;
    update_display();
  } 

  if (millis() - last_update >= INTERVAL_MQTT_PUBLISH) {
    last_update = millis();
    should_read = true;
  }

  if (should_read) {
    should_read = false;
    
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h) && t < 50.0 && h <= 100.0) {
      temperature = t;
      humidity = h;
      isFirstReadDone = true;
      
      wake_display();
      update_display();
      mqtt_connect(true);
      mqtt_publish();
    } else {
      Serial.println("Sensor error");
    }
  }
  if (isDisplayOn && !IsDewSleeping && (millis() - last_disp_on > INTERVAL_SLEEP_ON)) {
    sleep_display();
  }
  if (isDisplayOn && IsDewSleeping && (millis() - sleep_time > INTERVAL_SLEEP_ON_SCREEN_OFF)) {
    off_display();
  }
}