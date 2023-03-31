#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "freertos/queue.h"
#include <string.h>
#include <Wire.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  bool battery;
} struct_message;

struct_message incomingReadings;

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
  
  Serial.printf("Board ID %u: %u bytes\n %u battery\n", incomingReadings.id, len, incomingReadings.battery);
  int id = incomingReadings.id;
  id = (id << 1) | incomingReadings.battery;
  xQueueSend(queue, &id, (TickType_t)0); 
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Wire.begin(12, 14);

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 2000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    lastEventTime = millis();
    int id;
    if (uxQueueMessagesWaiting(queue)!=0) {
      xQueueReceive(queue, &id, 0);
      Wire.beginTransmission(0x0a);
      Wire.print(id);
      Wire.endTransmission();
      Serial.printf("I2C Transmission with ID: %x\n", id);
    }
  }
}




 
