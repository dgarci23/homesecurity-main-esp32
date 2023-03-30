#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <EEPROM.h>
#include "freertos/queue.h"
#include <string.h>
#include <Wire.h>
#include <initial.h>

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  initSPIFFS();

  // Load values saved in SPIFFS
  readFile();

  if(initWiFi()) {
    Serial.println("Setup finished");
  }
  else {
    initialConfig();
  }
}

void loop() {

}