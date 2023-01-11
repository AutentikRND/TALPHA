/*********
  HANIF FADHLURRAHMAN
*********/
//LIB...............................................................................................................
#include <Arduino.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SD.h>
#include <AnalogSmooth.h>
#include <Adafruit_Sensor.h>//Library Sensor Adafruit
#include <Adafruit_BME280.h>//Library BME280
#include "SparkFunHTU21D.h"
//Create an instance of the object
HTU21D sensor;
//Firebase..........................................................................................................
#define FIREBASE_HOST "diawanpremium-6d4a9.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "YfupaeSbkcqizmKzeFitYnu8SdkdTSmcxaNZKiai"
#define FIREBASE_JSONBUFFER_SIZE JSON_OBJECT_SIZE(128)
#define WINDOW_SIZE 10
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
Adafruit_BME280 bme; // I2C
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
DNSServer dns;
AnalogSmooth as = AnalogSmooth();
WiFiManager wifiManager;
FirebaseData firebaseData1;
//VAR
int INDEX = 0;
int VALUE = 0;
int SUM = 0;
int READINGS[WINDOW_SIZE];
int AVERAGED = 0;
float caltempC, calhumidity, tempC, humidity;
String formattedDate;
const int analogInput = A0;
float Vout = 0.00;
float Vin = 0.00;
float R1 = 10000.00; // resistance of R1 (10K)
float R2 = 1000.00; // resistance of R2 (1K)
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
File myFile;
char node_ID[] = "PR00094-01";


void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) { }
  wifiManager.setConfigPortalTimeout(300);
  wifiManager.autoConnect(node_ID);// rubah nama disini
  //bme.begin(0x76);
  sensor.begin();
  // Print ESP Local IP Address
  //Serial.println(WiFi.localIP());
  //Serial.println("connected...yeey :)");
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.begin();
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Serial.println("firebase connected");
    geturl();
    long rssi = WiFi.RSSI();
    Serial.print("RSSI: ");
    Serial.println(rssi);
    qualwifi = 2 * (rssi + 100);
    Serial.println(qualwifi);
    //tempC = bme.readTemperature();
    //humidity = bme.readHumidity();
    //t = a + b * tempC + c * (pow(tempC, 2)); //regresi temp
    //h = d + e * humidity + f * (pow(humidity, 2)); //regresi hum
    val = analogRead(analogInput);//reads the analog input
    float analogSmooth = as.smooth(val);
    Vout = (analogSmooth * 3.3) / 2095.00; // formula for calculating voltage out i.e. V+, here 5.00
    Vin = Vout * 3.9038; // formula for calculating voltage in i.e. GND
    Serial.println(Vin);
    currentValue = Vin * offsite3;
    if (currentValue > 4.1) {
      persen = 100;
    }
    else if (currentValue <= 4.1 && currentValue > 3.3) {
      persen = ((currentValue - 3.3) * (100 - 0)) / ((4.1 - 3.3) + 0);
    }
    else if (currentValue <= 3.3) {
      persen = 0;
    }
    readSensor();
    sendTofirebase(caltempC, calhumidity, tempC, humidity, persen);
    ESP.deepSleep(600e6);
  }
  else {
    ESP.restart();
  }
  // SoftwareSerial:
  //ss.begin(9600);
}

void loop() {
}

/*void writesd(){
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.print(formattedDate);
    myFile.print(" = Temp : ");
    myFile.print(temperatureC);
    myFile.print(" Batt : ");
    myFile.println(persen);
    //close the file:
    myFile.close();
    Serial.println("done.");
  } else {
     //if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  }*/

void readSensor() {
  //caltempC = tempC + t;
  //calhumidity = humidity + h;
  tempC = float(sensor.readTemperature());
  humidity = float (sensor.readHumidity());
  Serial.print("tempC=");
  Serial.println(tempC);
  Serial.print("hum=");
  Serial.println(humidity);
  te = (a * tempC) + (b * (pow(tempC, 2))) + c; //regresi temp
  Serial.println(te);
  h = (d * humidity) + (e * (pow(humidity, 2))) + f; //regresi hum
  caltempC = (tempC + te) * offsite1;
  calhumidity = (humidity + h) * offsite2;
  Serial.print("Persen = ");
  Serial.println(persen);
  Serial.print("Voltage :");
  Serial.println(currentValue);
  Serial.print(tempC);
  Serial.println("ºC");
}

void sendTofirebase(float caltempC, float calhumidity, float tempC, float humidity, int persen)
{
  WiFiClient client;
  HTTPClient http;
  String load = "{}";
  http.begin(client, link);
  waktu();
  String jsonStr = "";
  FirebaseJson json1;
  String formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);
  FirebaseJsonData jsonObj;
  json1.toString(jsonStr, true);
  Serial.print("Persen = ");
  json1.set("data1", caltempC);
  json1.set("data2", calhumidity);
  json1.set("data3", persen);
  json1.set("type", "esp32");
  json1.set("timestamp", formattedDate);
  json1.set("wifi", qualwifi);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"email\":\"" + email + "\",\"password\":\"" + pass + "\", \"userId\":\"" + userId + "\",\"idDevice\":\"" + idDevice + "\",\"value\":{\"data1\":" + caltempC + ",\"data2\":" + calhumidity + ",\"data3\":" + persen + ",\"wifi\":" + qualwifi + "}}");
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  if (httpResponseCode != 200) {
    if ( Firebase.push(firebaseData1, "clients/" + userId + "/devices/" + idDevice + "/values", json1)) {
    }
    else {
    }
  }

  else if (httpResponseCode == 500) {
    if ( Firebase.push(firebaseData1, "clients/" + userId + "/devices/" + idDevice + "/values", json1)) {
    }
    else {
    }
  }

  else {
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

void waktu() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
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
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
