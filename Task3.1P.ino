#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include "secrets.h"

const char wifiName[] = WIFI_NAME;
const char wifiPassword[] = WIFI_PASSWORD;
const int wifiReconnectDelay = 5000;
int wifiConnectionStatus = WL_IDLE_STATUS;

WiFiClient TCP_wifiClient;
const char serverAddress[] = "maker.ifttt.com";
const int serverPort = 80;  // Port for HTTP connection

HttpClient httpClientTcp = HttpClient(TCP_wifiClient, serverAddress, serverPort);

const int lightSensorPin = A0;
const int notBrightEnough = 1010;
int lightDetected;
const int delayBetweenReadings = 10000;

void setup() {
    Serial.begin(9600);
    while (!Serial);

    setupWiFi();
}

void loop() {
    lightDetected = analogRead(lightSensorPin);
    delay(10);
    Serial.println("\n>>> Light sensor value: " + String(lightDetected));

    const bool isReceivingSunlightNow = lightDetected > notBrightEnough;

    if (isReceivingSunlightNow) {
        triggerIFTTTEvent("/trigger/sunlight_received/with/key/b3A6-dH7dn13nOm3HZhUPa"); // Replace the key with your IFTTT Maker key
    } else {
        triggerIFTTTEvent("/trigger/sunlight_stopped/with/key/b3A6-dH7dn13nOm3HZhUPa"); // Replace the key with your IFTTT Maker key
    }

    delay(delayBetweenReadings);
}

void setupWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(wifiName, wifiPassword);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.println("Connected to WiFi");
}

void triggerIFTTTEvent(String eventPath) {
    httpClientTcp.get(eventPath);
    const int statusCode = httpClientTcp.responseStatusCode();
    String response = httpClientTcp.responseBody();

    Serial.println("\nHTTP response code: " + String(statusCode));
    Serial.println("IFTTT response: " + response);
}
