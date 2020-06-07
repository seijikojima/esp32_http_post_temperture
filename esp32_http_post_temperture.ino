#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const char* ssid = "";
const char* password = "";
WebServer server(80);

#include <DHT.h>

const int PIN_DHT = 4;
DHT dht(PIN_DHT,DHT11);

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11");
  dht.begin();
}

void loop(){

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    return;
  } else {
    Serial.print("Humidity:");
    Serial.print(h);
    Serial.print("%\t");
    Serial.print("Temperature:");
    Serial.print(t);
    Serial.println("*C");
  }

  delay(3000);
}
