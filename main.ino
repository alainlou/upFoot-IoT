extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
}
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

FPS_GT511C3 fps(3, 1); // (Arduino SS_RX = pin 4, Arduino SS_TX = pin 5)

const char * ssid = "eduroam";                  // The name of the Wi-Fi network that will be created
const char * username = "az2lou@uwaterloo.ca";  // The password required to connect to it, leave blank for an open network
const char * password = "";
unsigned long startTime;
unsigned long onTime;
unsigned long refTime;
unsigned long offRefTime;
WiFiClient client;
HTTPClient http;

void setup() {
  pinMode(2, INPUT);
  pinMode(0, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT);

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("Trying to connect to: ");
  Serial.println(ssid);

  wifi_station_disconnect();
  struct station_config wifi_config;

  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);
  strcpy((char*)wifi_config.password, password);
  wifi_station_set_config(&wifi_config);

  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();

  wifi_station_set_wpa2_enterprise_auth(1);
  wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen(password));

  wifi_station_connect();

  Serial.print("Status: ");
  Serial.println(wifi_station_get_connect_status());

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println(WiFi.localIP());
  }
  unsigned long refTime = millis();
  unsigned long offRefTime = millis();
  digitalWrite(0, HIGH);
}

void loop() {
  if (digitalRead(2) == LOW) {
    startTime = millis();
    onTime += (startTime - refTime);
    refTime = startTime;
  }
  if (digitalRead(5) == HIGH) {
    digitalWrite(4, HIGH);
    Serial.println("Request sent!");
    http.begin(client, "http://alainlou.api.stdlib.com/enghack@dev/verify/?name=Test&verified=false");
    int httpCode = http.GET();
    Serial.println(httpCode);
    Serial.println(http.getString());
    delay(250);
  } else {
    digitalWrite(4, LOW);
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(0, HIGH);
  } else {
    digitalWrite(0, LOW);
  }
  if (offRefTime > 8000) {
    if (onTime / offRefTime >= 0.75) {
      Serial.println("Busy mode detected");
    }
    else {
      Serial.println("Not Busy");
    }
    offRefTime = millis();
  }
}
