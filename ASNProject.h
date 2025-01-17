#ifndef ASNProject_h
#define ASNProject_h

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "OV2640.h"

// Camera settings
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

class ASNProject {
    public:
        static const char HEADER[];
        static const char BOUNDARY[];
        static const char CTNTTYPE[];
        static const int hdrLen;
        static const int bdrLen;
        static const int cntLen;
        
        ASNProject();
        
        // Rest API
        String send(const char* serverUrl, StaticJsonDocument<200>& jsonDoc);
        String get(const char* serverUrl);
        String update(const char* serverUrl, StaticJsonDocument<200>& jsonDoc);
        String getById(const char* serverUrl, const char* id);

        // ESP32Cam Streaming
        String esp32cam();
        void handleClient();
        void streamMJPEG();
        void captureJPG();
    
    private:
        WebServer server;
        OV2640 cam;

        void handleJPGStream();
        void handleJPG();
        void handleNotFound();
};

#endif