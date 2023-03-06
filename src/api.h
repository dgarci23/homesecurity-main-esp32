#ifndef API_H
#define API_H
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

void configureWiFi(String ssid);
String connectApi(String path, String method);
String getUser(String userId);
String getSensors(String userId);
void updateSensor(String userId, String sensorId);
#endif