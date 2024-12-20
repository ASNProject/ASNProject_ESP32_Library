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

ASNProject::ASNProject() {}

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
String ASNProject::getById(const char* serverUrl, char* id) {
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

