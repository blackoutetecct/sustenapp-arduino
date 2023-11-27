#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>

const char * ssid = "REDE WIFI";
const char * password = "SENHA";

IPAddress staticIP(192, 168, 18, 226);
IPAddress gateway(192, 168, 18, 1); 
IPAddress subnet(255, 255, 255, 0); 

AsyncWebServer server(80);

AsyncCallbackJsonWebHandler * handle = new AsyncCallbackJsonWebHandler(
    "/", [](AsyncWebServerRequest * request, JsonVariant & json) {
        StaticJsonDocument <200> jsonDoc;

        if (json["value"] == true) {

            jsonDoc["status"] = "200";
            jsonDoc["mensagem"] = "LAMPADA ACESA";

            String jsonString;
            serializeJson(jsonDoc, jsonString);
            request - > send(200, "application/json", jsonString);
        } else {
            jsonDoc["status"] = "200";
            jsonDoc["mensagem"] = "LAMPADA DESLIGADA";

            String jsonString;
            serializeJson(jsonDoc, jsonString);
            request - > send(200, "application/json", jsonString);
        }
    }
);

void handleRequest(AsyncWebServerRequest * request) {}

void setup() {
    Serial.begin(9600);
    
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    server.addHandler(handle);
    server.on("/", HTTP_POST, handleRequest);
    server.begin();
}

void loop() {
    
}