/***
  HANIF FADHLURRAHMAN this for ds18b20
***/
//LIB...............................................................................................................
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
//RANDOM............................................................................................................
#define power 4
            //User ID
            String userId = "185435b3-ffc4-45ef-a18b-02c70573a4f7";
            //ID Device
            String idDevice = "c6d03568-e70b-4486-b00f-3c1c712cc907";


            //Email Account
            String email = "autentik-rnd@gmail.com";
            //Email Password
            String pass = "YAF6wM";
            
            


String serverName = "http://diawan.io/api/get_url";
// GPIO where the DS18B20 is connected to
const int oneWireBus = 5;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
WiFiManager wifiManager;
//VAR
float temperatureC;
float caltemperatureC;
String formattedDate;
float a = 0, b = 0, c = 0, t = 0;
int analogInput = A0;
float Vout = 0.00;
float Vin = 0.00;
int val = 0;
String val1, val2;
float fval1 = 1.00;
float fval2 = 1.00;
float currentValue;
float offsite1 = 1;
float offsite2 = 1;
int persen;
int qualwifi = 0;
String link;
String name;
File myFile;
char node_ID[] = "TC Normal 1";


void setup() {
  // Start the Serial Monitor
  pinMode(power, OUTPUT);
  digitalWrite(power, HIGH);
  WiFi.forceSleepBegin();
  Serial.begin(115200);
  //readSensor();                                             //matikan ketika kalibrasi
  delay(1);
  WiFi.forceSleepWake();
  wifiManager.setConfigPortalTimeout(300);
  wifiManager.autoConnect(node_ID);
  if (WiFi.status() == WL_CONNECTED) {
    geturl();
    long rssi = WiFi.RSSI();
    //Serial.print("RSSI: ");
    //Serial.println(rssi);
    qualwifi = 2 * (rssi + 100);
    //Serial.println(qualwifi);
    //sendTofirebase(caltemperatureC, temperatureC, persen);  //matikan ketika kalibrasi
    digitalWrite(power, HIGH);                               //HIGH ketika kalibrasi
    ESP.deepSleep(600e6, WAKE_RFCAL);                       //matikan ketika kalibrasi
  }
  //else {
    //ESP.deepSleep(600e6, WAKE_RFCAL);                       //matikan ketika kalibrasi
  //}
}

void loop() {
  readSensor();
  delay(10);
  sendTofirebase(caltemperatureC, temperatureC, persen);
}

void readSensor() {
  sensors.begin();
  delay(750);
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);
  t = a + b * temperatureC + c * (pow(temperatureC, 2));  //regresi temp
  caltemperatureC = (temperatureC * offsite1) + t;
  val = analogRead(analogInput);  //reads the analog input
  //Serial.println(val);

  //Serial.println(currentValue);
  if (val > 788) {
    persen = 100;
  } else if (val <= 788 && val > 650) {
    persen = map(val, 650, 788, 0, 100);
  } else if (val <= 650) {
    persen = 0;
  }
  Serial.print("Persen = ");
  Serial.println(persen);
  // Serial.print("Voltage :");
  // Serial.println(currentValue);
  Serial.print(caltemperatureC);
  Serial.println("ºC");

}

void sendTofirebase(float caltemperatureC, float temperatureC, float persen) {
  WiFiClient client;
  HTTPClient http;
  String load = "{}";
  http.begin(client, link);
  String jsonStr = "";
  http.addHeader("Content-Type", "application/json");
  Serial.println(caltemperatureC);
  int httpResponseCode = http.POST("{\"email\":\"" + email + "\",\"password\":\"" + pass + "\", \"userId\":\"" + userId + "\",\"idDevice\":\"" + idDevice + "\",\"value\":{\"data1\":" + caltemperatureC + ",\"data2\":" + persen + ",\"wifi\":" + qualwifi + "}}");
  //Serial.print("HTTP Response code: ");
  //Serial.println(httpResponseCode);
  if (httpResponseCode != 200) {
  } else if (httpResponseCode == 500) {
  } else {
    load = http.getString();
    Serial.print(load);
    DynamicJsonDocument doc(1024);
    String input = load;
    deserializeJson(doc, input);
    JsonObject obj = doc.as<JsonObject>();
    offsite1 = obj["result"]["offsite"]["offsite_value_data1"].as<float>();
    offsite2 = obj["result"]["offsite"]["offsite_value_data2"].as<float>();
    //name = obj["result"]["name"].as<String>();
    //Serial.println(offsite1);
    //Serial.println(offsite2);
    //Serial.println(name);
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
    //Serial.print("HTTP Response code: ");
    //Serial.println(httpResponseCode);
    String payload = http.getString();
    //Serial.println(payload);
    DynamicJsonDocument doc(1024);
    String input = payload;
    deserializeJson(doc, input);
    JsonObject obj = doc.as<JsonObject>();
    link = obj["url"]["push"].as<String>();
    offsite1 = obj["offsite"]["offsite_data1"].as<float>();
    offsite2 = obj["offsite"]["offsite_data2"].as<float>();
    a = obj["regression"]["data1"]["a"].as<float>();
    b = obj["regression"]["data1"]["b"].as<float>();
    c = obj["regression"]["data1"]["c"].as<float>();
    // Serial.println(offsite1);
    // Serial.println(offsite2);
    // Serial.print("urlGet ");
    // Serial.println(link);
  } else {
    // Serial.print("Error code: ");
    // Serial.println(httpResponseCode);
  }
  http.end();
}
