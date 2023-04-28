#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <api.h>
#include <EEPROM.h>
#include "freertos/queue.h"
#include <string.h>
#include <Wire.h>
#include <initial.h>

#define TRIGGER 0
#define BATTERY 1
#define LOW_BATTERY 2

typedef struct struct_message {
  int id;
  int battery;
} struct_message;

QueueHandle_t queue = xQueueCreate(10, sizeof(struct_message));

void receiveEvent(int len) {
  struct_message message;
  int id = Wire.read();
  int battery = Wire.read();
  message.id = id;
  message.battery = battery;
  Serial.printf("id: %x, battery: %x\n", id, battery);
  xQueueSend(queue, &message, (TickType_t)0);
}

void setup() {
  Serial.begin(115200);

  initSPIFFS();

  // Load values saved in SPIFFS
  readFile();

  if(initWiFi()) {
    // I2C Setup
    Serial.printf("I2C Setup.\n");
    Wire.begin(0x0a, 18, 19, 0U);
    Wire.onReceive(receiveEvent);
  }
  else {
    initialConfig();
  }
}
  
void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 2000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    lastEventTime = millis();
    struct_message message;
    if (uxQueueMessagesWaiting(queue)!=0) {
      xQueueReceive(queue, &message, 0);
      if (message.battery == TRIGGER) {
        Serial.printf("Sensor %x triggered.\n", message.id);
        triggerSensor(getUserId(), String(message.id));
      } else if (message.battery == BATTERY) {
        Serial.printf("Sensor %x. High battery.\n", message.id);
        batterySensor(getUserId(), String(message.id), "true");
      } else {
        Serial.printf("Sensor %x. Low battery.\n", message.id);
        batterySensor(getUserId(), String(message.id), "false");
      }
    }
  }
}