#include "ThingsBoard.h"
#include <ESP8266WiFi.h>
#include <LibPrintf.h>
#include "DHTesp.h"

DHTesp DHT;

#define DHT22_PIN 2
#define UCHAR unsigned char

#define WIFI_AP             "wifi name"
#define WIFI_PASSWORD       "-------"
#define TOKEN               "access_token"
#define THINGSBOARD_SERVER  "185.8.175.221"
#define SERIAL_DEBUG_BAUD   9600

WiFiClient espClient;
ThingsBoard tb(espClient);
int status = WL_IDLE_STATUS;
char s[41];

void setup() {
  DHT.setup(DHT22_PIN, DHTesp::DHT22);
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  InitWiFi();
}

void loop() {
  delay(DHT.getMinimumSamplingPeriod());

  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  float humidity = DHT.getHumidity();
  float temperature = DHT.getTemperature();
  snprintf(s, 41, "{\"temperature\":%6.2f,\"humidity\":%6.2f}", temperature, humidity);
  Serial.println(s);
  Serial.println(DHT.getStatusString());
  
  if(temperature == temperature && humidity == humidity) {
    Serial.println("Sending data...");
    tb.sendTelemetryJson(s);
  }

  tb.loop();
  delay(2000);
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.config(IPAddress(192, 168, 1, 44), IPAddress(8, 8, 8, 8), IPAddress(192, 168, 1, 28), IPAddress(255, 255, 255, 0));
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}
