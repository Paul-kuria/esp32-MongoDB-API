#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include <ArduinoJson.h>
#include <DHT.h>
#include "Secrets.h"
#include "time.h"

#define DHTpin 22
#define LEDGREENPIN 23
#define LEDREDPIN 21

// Initialize DHT object
DHT dht(DHTpin, DHT11);

// Initialize Time
String epochTime;
unsigned long dataMillis = 0;
const long gmtOffset_sec = (3*3600);
long duration = 60000 * 5; // 5 minutes

const char* ntpServer = "pool.ntp.org";
const char* z;
StaticJsonDocument<500> doc;  // A little more than 500 bytes in the stack.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();
  pinMode(LEDGREENPIN, OUTPUT);
  digitalWrite(LEDGREENPIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  Serial.print("Connected with IP Address: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, 0, ntpServer);
}

const char* getTime(){
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
    return(0);
  }
  z = (&timeinfo,"%Y-%M-%d %H:%M:%S");
  return z;
}

void POSTData()
{
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(serverName);
    http.addHeader(
      "Content-Type",
      "application/json"
    );
    String json;
    serializeJson(doc, json);
    Serial.println(json);

    int httpResponseCode = http.POST(json);
    Serial.println(httpResponseCode);

    if (httpResponseCode == 200){
      Serial.println("Data uploaded.");
      digitalWrite(LEDGREENPIN, HIGH);
      delay(200);
      digitalWrite(LEDGREENPIN, LOW);
    }
    else{
      Serial.print("Error uploading data");
      Serial.println();
      digitalWrite(LEDREDPIN, HIGH);
      delay(200);
      digitalWrite(LEDREDPIN, LOW);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - dataMillis > duration || dataMillis == 0)
  {
    dataMillis = millis();
    // epochTime = getTime();
    const char* epoch = getTime();
    Serial.print("Epoch time: ");
    Serial.println(epoch);
    

    // Float temperature + Humidity
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    delay(5);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("Humidity: ");
    Serial.print(humidity);

    // Document
    // The single quote is used to define single character and double quotes is used to define string literal
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["timestamp"] = epoch;

    Serial.println("Uploading data... ");
    POSTData();
  }
}

