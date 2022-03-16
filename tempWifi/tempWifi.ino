#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#define DHTPIN 4    
#define DHTTYPE DHT11 

#define API_KEY "API_KEY"
#define API_URL "https://api.thingspeak.com/update"

#define WIFI_SSID "SSID"
#define WIFI_PASS "PASS"


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(WIFI_SSID,WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting for connection");
  }
  
}

void sendToServer(float t, float h) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    if( http.begin(client, String(API_URL) + "?api_key=" + String(API_KEY) + "&field1=" + h + "&field2=" + t)){
      http.addHeader("Content-Type", "text/plain");
      int resultCode = http.GET(); 
      if(resultCode > 0){
          Serial.println("Wysłano");
      }
      else{
        Serial.println("Błąd wysyłania");
      }
      http.end();
    }
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Nie można odczytać sensora!"));
    return;
  }

  sendToServer(t,h);
}
