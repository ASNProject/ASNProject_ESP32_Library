#ifndef ASNProject_h
#define ASNProject_h

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class ASNProject {
    public:
        ASNProject();
        String send(const char* serverUrl, StaticJsonDocument<200>& jsonDoc);
};

#endif