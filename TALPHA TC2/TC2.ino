/*********
  HANIF FADHLURRAHMAN
*********/
//LIB...............................................................................................................
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "SparkFunHTU21D.h"
#include <ArduinoJson.h>
//Create an instance of the object
HTU21D sensor;
//RANDOM............................................................................................................
//User ID...........................................................................................................
//User ID
String userId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//ID Device
String idDevice = "717c1733-ef0e-45c2-bb1e-74d7ae5b21d0";


//Email Account
String email = "talpha.autentik@gmail.com";
//Email Password
String pass = "tU6EXs";



String serverName = "http://diawan.io/api/get_url";
WiFiManager wifiManager;
//VAR
float caltempC, calhumidity, tempC, humidity;
const int analogInput = A0;
float Vout = 0.00;
float Vin = 0.00;
int val = 0;
float currentValue;
float offsite1 = 1;
float offsite2 = 1;
float offsite3 = 1;
float a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, t = 0, h = 0, te = 0;
int persen;
int qualwifi = 0;
String link;
String name;
char node_ID[] = "PR00094-01";


void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  WiFi.forceSleepBegin();
  sensor.begin();
  WiFi.forceSleepWake();
  wifiManager.setConfigPortalTimeout(300);
  if (wifiManager.autoConnect(node_ID)) {
    geturl();
    long rssi = WiFi.RSSI();
    Serial.print("RSSI: ");
    Serial.println(rssi);
    qualwifi = 2 * (rssi + 100);
    Serial.println(qualwifi);
    readSensor();
    sendTofirebase(caltempC, calhumidity, persen);
    ESP.deepSleep(50e6, WAKE_RFCAL);  //600=10mnt 300=5mnt
  } else {
    ESP.deepSleep(50e6, WAKE_RFCAL);
  }
}

void loop() {
}

void readSensor() {
  sensor.begin();
  tempC = float(sensor.readTemperature());
  humidity = float(sensor.readHumidity());
  Serial.print("tempC=");
  Serial.println(tempC);
  Serial.print("hum=");
  Serial.println(humidity);
  te = (a * tempC) + (b * (pow(tempC, 2))) + c;  //regresi temp
  Serial.println(te);
  h = (d * humidity) + (e * (pow(humidity, 2))) + f;  //regresi hum
  caltempC = (tempC + te) * offsite1;
  calhumidity = (humidity + h) * offsite2;
  Serial.print("Persen = ");
  Serial.println(persen);
  Serial.print("Voltage :");
  Serial.println(currentValue);
  Serial.print(tempC);
  Serial.println("ºC");
  val = analogRead(analogInput);  //reads the analog input
  Serial.println(val);
  if (val > 788) {
    persen = 100;
  } else if (val <= 788 && val > 650) {
    persen = map(val, 650, 788, 0, 100);
  } else if (val <= 650) {
    persen = 0;
  }
  Serial.print("Persen = ");
  Serial.println(persen);
}

void sendTofirebase(float caltempC, float calhumidity, float persen) {
  WiFiClient client;
  HTTPClient http;
  String load = "{}";
  http.begin(client, link);
  String jsonStr = "";
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"email\":\"" + email + "\",\"password\":\"" + pass + "\", \"userId\":\"" + userId + "\",\"idDevice\":\"" + idDevice + "\",\"value\":{\"data1\":" + caltempC + ",\"data2\":" + calhumidity + ",\"data3\":" + persen + ",\"wifi\":" + qualwifi + "}}");
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  if (httpResponseCode != 200) {
  } else {
    load = http.getString();
    Serial.print(load);
    DynamicJsonDocument doc(1024);
    String input = load;
    deserializeJson(doc, input);
    JsonObject obj = doc.as<JsonObject>();
    offsite1 = obj["result"]["offsite"]["offsite_value_data1"].as<float>();
    offsite2 = obj["result"]["offsite"]["offsite_value_data2"].as<float>();
    offsite3 = obj["result"]["offsite"]["offsite_value_data3"].as<float>();
    name = obj["result"]["name"].as<String>();
    Serial.println(offsite1);
    Serial.println(offsite2);
    Serial.println(offsite3);
    Serial.println(name);
    if (obj["result"]["restart"].as<int>() == 1) {
      ESP.restart();
    }
    if (obj["result"]["reset"].as<int>() == 1) {
      wifiManager.resetSettings();
      ESP.restart();
    }
    http.end();
  }
}

void geturl() {
  WiFiClient client;
  HTTPClient http;
  String serverPath = serverName + "/" + idDevice;
  http.begin(client, serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
    DynamicJsonDocument doc(1024);
    String input = payload;
    deserializeJson(doc, input);
    JsonObject obj = doc.as<JsonObject>();
    link = obj["url"]["push"].as<String>();
    offsite1 = obj["offsite"]["offsite_data1"].as<float>();
    offsite2 = obj["offsite"]["offsite_data2"].as<float>();
    offsite3 = obj["offsite"]["offsite_data3"].as<float>();
    Serial.println(offsite1);
    Serial.println(offsite2);
    Serial.println(offsite2);
    Serial.print("urlGet ");
    Serial.println(link);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
