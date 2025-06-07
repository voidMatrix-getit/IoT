#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include "config.h"

#define DHTPIN 4         
#define DHTTYPE DHT11    
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float temperatureC = dht.readTemperature();    // Read temperature in Celsius
    float temperatureF = dht.readTemperature(true); // Read temperature in Fahrenheit
    float humidity = dht.readHumidity();          // Read humidity
  
    if (isnan(temperatureC) || isnan(temperatureF) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Creating JSON payload
    String payload = String("{\"temperatureC\":") + String(temperatureC) + 
                     ", \"temperatureF\":" + String(temperatureF) + 
                     ", \"humidity\":" + String(humidity) + "}";

    
    HTTPClient http;
    String url = String(UBIDOTS_URL);
    http.begin(url.c_str());
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Auth-Token", UBIDOTS_TOKEN);

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      Serial.println("Data sent to Ubidots successfully!");
    } else {
      Serial.print("Error sending data to Ubidots: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Wi-Fi not connected. Trying to reconnect...");
    WiFi.begin(ssid, password);
  }

  delay(100);
}


