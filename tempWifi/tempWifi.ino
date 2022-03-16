#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#define DHTPIN 4
#define DHTTYPE DHT11

#define SEND_LED_PIN 2
#define R_LED_PIN 12
#define G_LED_PIN 13
#define B_LED_PIN 15

#define API_KEY "33LU8KGICUXUBCZK"
#define API_URL "https://api.thingspeak.com/update"

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PASS"

DHT dht(DHTPIN, DHTTYPE);

void setColor(int r, int g, int b)
{
  int rr = 255 - r;
  int gg = 255 - g;
  int bb = 255 - b;

  analogWrite(R_LED_PIN, rr);
  analogWrite(G_LED_PIN, gg);
  analogWrite(B_LED_PIN, bb);
}

void setup()
{
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  dht.begin();

  pinMode(R_LED_PIN, OUTPUT);
  pinMode(G_LED_PIN, OUTPUT);
  pinMode(B_LED_PIN, OUTPUT);
  pinMode(SEND_LED_PIN, OUTPUT);
  digitalWrite(SEND_LED_PIN, HIGH);

  setColor(255, 0, 0);
  while (WiFi.status() != WL_CONNECTED)
  {
    setColor(0, 0, 0);
    delay(300);
    Serial.println("Waiting for connection");
    setColor(0, 150, 255);
    delay(200);
  }
  setColor(0, 255, 0);
}

void sendToServer(float t, float h)
{
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure();
  String url = String(API_URL) + "?api_key=" + String(API_KEY) + "&field1=" + h + "&field2=" + t;

  if (http.begin(client, url))
  {
    http.addHeader("Content-Type", "text/plain");
    int resultCode = http.GET();

    if (resultCode > 0)
    {
      Serial.println("Wysłano - " + String(resultCode) + " | " + url);
    }
    else
    {
      Serial.println("Błąd wysyłania");
    }

    http.end();
  }
}

void setTemperatureColor(float t)
{
  setColor(0, 0, 0);
  if (t <= 19.0)
    setColor(0, 80, 252);
  else if (t > 19.0 && t <= 23.0)
    setColor(0, 255, 69);
  else if (t > 23.0 && t <= 30.0)
    setColor(235, 252, 3);
  else if (t > 30.0)
    setColor(252, 10, 0);
}

void loop()
{
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Nie można odczytać sensora!"));
    return;
  }
  digitalWrite(SEND_LED_PIN, LOW);
  sendToServer(t, h);
  digitalWrite(SEND_LED_PIN, HIGH);
  // setTemperatureColor(t);
  setColor(0, 0, 0);
}
