#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <api.h>
#include <EEPROM.h>
#include "freertos/queue.h"
#include <string.h>
#include <Wire.h>

#define SSID "ND-guest"
#define USERID "dgarci23"

String ssid;
QueueHandle_t queue = xQueueCreate(10, sizeof(int));

void getConfig() {
  ssid = EEPROM.readString(0);
}

void saveInitialConfig() {
  EEPROM.writeString(0, SSID);
  EEPROM.commit();
  Serial.println("Configuration data persisted.");
}

void receiveEvent(int len) {
  int id = 0;
  while (Wire.available()){
    id = (id << 8) | Wire.read();
  }
  xQueueSend(queue, &id, (TickType_t)0);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  

  EEPROM.begin(32);
  saveInitialConfig(); // Should be passed in the INITIAL CONFIG
  getConfig();

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to the WiFi..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  // I2C Setup
  Wire.begin(0x0a, 12, 14, 0U);
  Wire.onReceive(receiveEvent);
}


 
void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 2000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    lastEventTime = millis();
    int id;
    bool battery;
    if (uxQueueMessagesWaiting(queue)!=0) {
      xQueueReceive(queue, &id, 0);
      battery = id & 0x1;
      id = id >> 1;
      if (battery) {
        Serial.printf("Sensor %x triggered.\n", id);
        triggerSensor(USERID, String(id));
      } else {
        Serial.printf("Sensor %x. Low battery.\n", id);
      }
    }
  }
}