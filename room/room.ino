extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
}
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

const char * ssid = "eduroam";
const char * username = "az2lou@uwaterloo.ca";
const char * password = "";
unsigned long startTime;
unsigned long onTime;
unsigned long refTime = 0;
unsigned long counter = 0;
unsigned long curr = 0;

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

  digitalWrite(0, HIGH);
}

void loop() {
  if (digitalRead(2) == LOW) {
    counter += 1;
  }
  if (digitalRead(5) == HIGH) {
    digitalWrite(4, HIGH);
    Serial.println("Request sent!");
    http.begin(client, "http://alainlou.api.stdlib.com/enghack@dev/update/?space=E7%20Atrium&traffic=2");
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
  if (curr > 400000) {     
    if (counter >= 250000) {
      http.begin(client, "http://alainlou.api.stdlib.com/enghack@dev/updateRooms/?room=4324&taken=true");
      int httpCode = http.GET();
      Serial.println(httpCode);
      Serial.println("Taken");
      delay(250);
    } else {
      http.begin(client, "http://alainlou.api.stdlib.com/enghack@dev/updateRooms/?room=4325&taken=false");
      int httpCode = http.GET();
      Serial.println(httpCode);
      Serial.println("Free");
      delay(250);
    }
    counter = 0;
    curr = 0;
  }
  ++curr;
}
