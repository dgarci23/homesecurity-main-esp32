#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <api.h>
#include <EEPROM.h>
#include "freertos/queue.h"
#include <string.h>
#include <Wire.h>
#include <initial.h>

#define USERID "dgarci23"

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
  // Serial port for debugging purposes
  Serial.begin(115200);

  initSPIFFS();

  // Load values saved in SPIFFS
  readFile();

  if(initWiFi()) {
    Serial.println("Setup finished");
    // I2C Setup
    Wire.begin(0x0a, 12, 14, 0U);
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
      if (message.battery == 0) {
        Serial.printf("Sensor %x triggered.\n", message.id);
        triggerSensor(USERID, String(message.id));
      } else if (message.battery == 1) {
        batterySensor(USERID, String(message.id), "true");
      } else {
        batterySensor(USERID, String(message.id), "false");
        Serial.printf("Sensor %x. Low battery.\n", message.id);
      }
    }
  }
}