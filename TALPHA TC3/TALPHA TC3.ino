
//LIB...............................................................................................................
#include <Arduino.h>
//#include <WiFi.h>
//#include <HTTPClient.h>
//#include <WebServer.h>
//#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <SPI.h>
#include <Adafruit_MAX31865.h>
#include <pt100rtd.h>
#include <ArduinoJson.h>
//RANDOM............................................................................................................
// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF 430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
//#define RNOMINAL 100.0
// Like, duh.
#define C2F(c) ((9 * c / 5) + 32)
// init the Pt100 table lookup module
pt100rtd PT100 = pt100rtd();
// Use software SPI: CS, SDI, SDO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(4, 13, 12, 14);
//User ID
String userId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//ID Device
String idDevice = "fb734f8a-f8bb-42de-9411-3f3440f868e6";


//Email Account
String email = "talpha.autentik@gmail.com";
//Email Password
String pass = "5k7ZHf";

String serverName = "http://diawan.io/api/get_url";
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
String linkdiawan;
String name;
//File myFile;
char node_ID[] = "MAX3865";

void setup() {
  // Start the Serial Monitor
  //WiFi.forceSleepBegin();
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  thermo.begin(MAX31865_3WIRE);
  delay(1);
  //WiFi.forceSleepWake();
  wifiManager.setConfigPortalTimeout(300);
  wifiManager.autoConnect(node_ID);
  if (WiFi.status() == WL_CONNECTED) {
    geturl();
    long rssi = WiFi.RSSI();
    //Serial.print("RSSI: ");
    //Serial.println(rssi);
    qualwifi = 2 * (rssi + 100);
    //Serial.println(qualwifi);
    //sendTofirebase(caltemperatureC, temperatureC, persen);
    //ESP.deepSleep(600e6, WAKE_RFCAL);
  } else {
    //ESP.deepSleep(600e6, WAKE_RFCAL);
  }
}

void loop() {
  readSensor();
  delay(3000);
  sendTofirebase(caltemperatureC, temperatureC);
}

void readSensor() {
  uint16_t rtd, ohmsx100;
  uint32_t dummy;
  float ohms, Tlut;
  float Tcvd, Tcube, Tpoly, Trpoly;
  rtd = thermo.readRTD();
  Serial.print("RTD value: ");
  Serial.println(rtd);
  // Use uint16_t (ohms * 100) since it matches data type in lookup table.
  dummy = ((uint32_t)(rtd << 1)) * 100 * ((uint32_t)floor(RREF));
  dummy >>= 16;
  ohmsx100 = (uint16_t)(dummy & 0xFFFF);

  // or use exact ohms floating point value.
  ohms = (float)(ohmsx100 / 100) + ((float)(ohmsx100 % 100) / 100.0);

  Serial.print("rtd: 0x");
  Serial.print(rtd, HEX);
  Serial.print(", ohms: ");
  Serial.println(ohms, 2);

  temperatureC = PT100.celsius(ohmsx100);                   // NoobNote: LUT== LookUp Table
  Tcvd = PT100.celsius_cvd(ohms);                   // Callendar-Van Dusen calc
  Tcube = PT100.celsius_cubic(ohms);                // Cubic eqn calc
  Tpoly = PT100.celsius_polynomial(ohms);           // 5th order polynomial
  Trpoly = PT100.celsius_rationalpolynomial(ohms);  // ugly rational polynomial quotient

  t = a + b * temperatureC + c * (pow(temperatureC, 2));  //regresi temp
  caltemperatureC = (temperatureC * offsite1) + t;
  val = analogRead(analogInput);  //reads the analog input
  //Serial.println(val);

  //Serial.println(currentValue);
  // if (val > 788) {
  //   persen = 100;
  // } else if (val <= 788 && val > 650) {
  //   persen = map(val, 650, 788, 0, 100);
  // } else if (val <= 650) {
  //   persen = 0;
  // }
  // Serial.print("Persen = ");
  // Serial.println(persen);
  // Serial.print("Voltage :");
  // Serial.println(currentValue);
  Serial.print(caltemperatureC);
  checkFault() ;
  Serial.println("ºC");
  Serial.println();
}

void sendTofirebase(float caltemperatureC, float temperatureC) {
  WiFiClient client;
  HTTPClient http;
  String load = "{}";
  http.begin(client, linkdiawan);
  String jsonStr = "";
  http.addHeader("Content-Type", "application/json");
  Serial.println(caltemperatureC);
  int httpResponseCode = http.POST("{\"email\":\"" + email + "\",\"password\":\"" + pass + "\", \"userId\":\"" + userId + "\",\"idDevice\":\"" + idDevice + "\",\"value\":{\"data1\":" + caltemperatureC + ",\"wifi\":" + qualwifi + "}}");
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

void checkFault(void) {
  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x");
    Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage");
    }
    thermo.clearFault();
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
    linkdiawan = obj["url"]["push"].as<String>();
    offsite1 = obj["offsite"]["offsite_data1"].as<float>();
    offsite2 = obj["offsite"]["offsite_data2"].as<float>();
    a = obj["regression"]["data1"]["a"].as<float>();
    b = obj["regression"]["data1"]["b"].as<float>();
    c = obj["regression"]["data1"]["c"].as<float>();
    // Serial.println(offsite1);
    // Serial.println(offsite2);
    // Serial.print("urlGet ");
    // Serial.println(linkdiawan);
  } else {
    // Serial.print("Error code: ");
    // Serial.println(httpResponseCode);
  }
  http.end();
}
