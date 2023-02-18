#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <secrets.h>

WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());
}

void getLightDefinition(){
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client){
    client -> setCACert(rootCACertificate);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WifiClientSecure *client is
      HTTPClient https;

      if (https.begin(*client, AtlasAPIEndpoint)){
        /*Headers Required for Data API*/
        https.addHeader("Content-Type", "application/json");
        https.addHeader("api-key", AtlasAPIEndpoint);

         // Generate an InsertOne Payload
         DynamicJsonDocument payload (1024);
         payload["dataSource"] = "myDesk";
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}