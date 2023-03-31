#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#include <FS.h>

#ifndef INITIAL_H
#define INITIAL_H


String getUserId();
void initSPIFFS();
String readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
bool initWiFi();
void initialConfig();
void readFile();

#endif