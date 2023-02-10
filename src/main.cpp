#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <api.h>
#include <EEPROM.h>

#define SSID "ND-guest"

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  int value;
} struct_message;

struct_message incomingReadings;

String ssid;
int update[] = {0,0};

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
  update[incomingReadings.id] = 1;

}

void getConfig() {
  ssid = EEPROM.readString(0);
}

void saveInitialConfig() {
  EEPROM.writeString(0, SSID);
  EEPROM.commit();
  Serial.println("Configuration data persisted.");
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  EEPROM.begin(32);
  saveInitialConfig(); // Should be passed in the INITIAL CONFIG
  getConfig();

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

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
    if (update[1]==1) {
      updateSensor("dgarci23", "1", "triggered");
      update[1] = 0;
    }
  }
}