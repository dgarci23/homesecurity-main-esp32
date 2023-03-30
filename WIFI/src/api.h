#ifndef API_H
#define API_H
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

void configureWiFi(String ssid);
String connectApi(String path, String method);
String getUser(String userId);
String getSensors(String userId);
void triggerSensor(String userId, String sensorId);
String batterySensor(String userId, String sensorId, String batteryStatus);
#endif