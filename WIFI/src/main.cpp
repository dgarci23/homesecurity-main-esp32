#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <api.h>
#include <EEPROM.h>
#include "freertos/queue.h"
#include <string.h>
#include <Wire.h>

#define SSID "ND-guest"

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  int value;
} struct_message;

struct_message incomingReadings;

String ssid;
QueueHandle_t queue = xQueueCreate(10, sizeof(int));

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  // Copies the sender mac address to a string
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  Serial.printf("Board ID %u: %u bytes\n %u value", incomingReadings.id, len, incomingReadings.value);
  Serial.println();
  short id = incomingReadings.id;
  xQueueSend(queue, &id, (TickType_t)0); 
}

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
  //Serial.printf("Device id: %x\n", id);
}

void requestEvent()
{
  static char c = '0';
  Serial.println("Transmission");
  Wire.print(c);
  if (c > 'z')
    c = '0';
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
  Wire.begin(0x0a ,12,14, 0U);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}


 
void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 2000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    lastEventTime = millis();
    int id;
    if (uxQueueMessagesWaiting(queue)!=0) {
      xQueueReceive(queue, &id, 0);
      Serial.printf("Sensor %x triggered\n", id);
      updateSensor("dgarci23", String(id));
    }
  }
}