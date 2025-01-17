// Copyright 2024 ariefsetyonugroho
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     https://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ASNProject.h"

// Camera streaming setup
const char ASNProject::HEADER[] = "HTTP/1.1 200 OK\r\n" \
                                      "Access-Control-Allow-Origin: *\r\n" \
                                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";
const char ASNProject::BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n";
const char ASNProject::CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
const int ASNProject::hdrLen = strlen(HEADER);
const int ASNProject::bdrLen = strlen(BOUNDARY);
const int ASNProject::cntLen = strlen(CTNTTYPE);

ASNProject::ASNProject() : server(80) {}
///----- REST API -----///
// POST Data
String ASNProject::send(const char* serverUrl, StaticJsonDocument<200>& jsonDoc) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");

        // Serialisasi JSON to string
        String jsonData;
        serializeJson(jsonDoc, jsonData);

        // send request POST
        int httpResponseCode = http.POST(jsonData);

        // Get response server
        String response;
        if (httpResponseCode > 0) {
            response = http.getString();
        } else {
            response = "Error: " + String(http.errorToString(httpResponseCode));
        }

        http.end();
        return response;
    } else {
        return "Error: Wifi not connected.";
    }
}

// GET Data
String ASNProject::get(const char* serverUrl) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        int httpResponseCode = http.GET();

        String response;
        if (httpResponseCode > 0) {
            response = http.getString();
        } else {
            response = "Error: " + String(http.errorToString(httpResponseCode));
        }

        http.end();
        return response;
    } else {
        return "Error: Wifi not connected.";
    }
}

// UPDATE Data
String ASNProject::update(const char* serverUrl, StaticJsonDocument<200>& jsonDoc) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");

        String jsonData;
        serializeJson(jsonDoc, jsonData);

        int httpResponseCode = http.POST(jsonData);

        String response;
        if (httpResponseCode > 0) {
            response = http.getString();
        } else {
            response = "Error: " + String(http.errorToString(httpResponseCode));
        }

        http.end();
        return response;
    } else {
        return "Error: Wifi not connected.";
    }
}

/// GET Data by ID
String ASNProject::getById(const char* serverUrl, const char* id) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(String(serverUrl) + "/" + String(id));
        int httpResponseCode = http.GET();

        String response;
        if (httpResponseCode > 0) {
            response = http.getString();
        } else {
            response = "Error: " + String(http.errorToString(httpResponseCode));
        }

        http.end();
        return response;
    } else {
        return "Error: Wifi not connected.";
    }
}

///----- Camera ESP32CAM -----///
String ASNProject::esp32cam() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;

    cam.init(config);

    IPAddress ip = WiFi.localIP();\
    server.on("/mjpeg/1", HTTP_GET, std::bind(&ASNProject::handleJPGStream, this));
    server.on("/jpg", HTTP_GET, std::bind(&ASNProject::handleJPG, this));
    server.onNotFound(std::bind(&ASNProject::handleNotFound, this));
    server.begin();

    String streamLink = "http://" + ip.toString() + "/mjpeg/1";
    return streamLink;
}

void ASNProject::handleClient() {
  server.handleClient();
}

void ASNProject::handleJPGStream() {
  WiFiClient client = server.client();
  client.write(HEADER, hdrLen);
  client.write(BOUNDARY, bdrLen);

  while (true) {
    if (!client.connected()) break;
    cam.run();
    int s = cam.getSize();
    client.write(CTNTTYPE, cntLen);
    char buf[32];
    sprintf(buf, "%d\r\n\r\n", s);
    client.write(buf, strlen(buf));
    client.write((char*)cam.getfb(), s);
    client.write(BOUNDARY, bdrLen);
  }
}

void ASNProject::handleJPG() {
  WiFiClient client = server.client();
  cam.run();
  if (!client.connected()) return;
  
  const char JHEADER[] = "HTTP/1.1 200 OK\r\n" \
                         "Content-disposition: inline; filename=capture.jpg\r\n" \
                         "Content-type: image/jpeg\r\n\r\n";
  client.write(JHEADER, strlen(JHEADER));
  client.write((char*)cam.getfb(), cam.getSize());
}

void ASNProject::handleNotFound() {
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text/plain", message);
}

