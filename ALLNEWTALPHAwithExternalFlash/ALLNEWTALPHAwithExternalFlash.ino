/*
   firmware version 2.0.3
   developed babynoy
*/
//ALL LIBRARY
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "SdFat.h"
#include "Adafruit_SPIFlash.h"
#include <SPI.h>
#include "SparkFunHTU21D.h"
HTU21D sensor;
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <FlickerFreePrint.h>
#include <Fonts/SevenSegment50pt7b.h>
//ALL DEFINE
#define SEALEVELPRESSURE_HPA (1013.25)
#define TFT_CS D8
#define TFT_RST D3
#define TFT_DC D4
#define bat A0
//void postFileContent(const char * path );
#define FILE_LINE_LENGTH 10000  // a line has 80 chars
char txtLine[FILE_LINE_LENGTH];
String txtstring;
char postdata[FILE_LINE_LENGTH];
bool readCondition = true;  // Has to be defined somewhere to trigger SD read
WiFiClient client;

//LIBRARY NEEDED
Adafruit_FlashTransport_SPI flashTransport(10, SPI);  // Set CS and SPI interface
Adafruit_SPIFlash flash(&flashTransport);
FatFileSystem fatfs;
//File myFile;
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
FlickerFreePrint<Adafruit_ILI9341> Data1(&tft, ILI9341_WHITE, ILI9341_BLACK);
FlickerFreePrint<Adafruit_ILI9341> Data2(&tft, ILI9341_WHITE, ILI9341_BLACK);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
//Adafruit_BME280 bme; // I2C
DNSServer dns;
WiFiManager wifiManager;
//AnalogSmooth as = AnalogSmooth();

//User ID
String userId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//ID Device
String idDevice = "c33fdec6-2bdb-4d21-9a3e-7c37c33cc51a";
//Email Account
String email = "talpha.autentik@gmail.com";
//Email Password
String pass = "CwWl1c";

char dataHari[7][12] = { "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY" };
String hari;
String namahari;
String nomortanggal;
String nilaiwaktu;
String lasttime;
int tanggal, bulan, tahun, menit, detik;
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned int startTime = millis();
char t[32];
unsigned int timeout = 120;  // seconds to run for
File root;

//MicroSD CARD ADAPTER
String dataMessage;

String serverName = "http://diawan.io/api/get_url";
String val1, val2, val3, val4, val5, val6, val7;
String formattedDate;
String link1 = "http://diawan.io/api/upload-json";
String link;
String waktu, jam;
String name;

//INTEGER
int qualwifi = 0;
int saklar;
int q = 0;
int interval = 24;
int persen;
int val = 0;
float caltempcclear;

//FLOAT
float voltageValue, currentValue;
float fval1 = 1;
float fval2 = 1;
float fval3 = 1;
float offsite1 = 1;
float offsite2 = 1;
float koreksi1 = -2;
float koreksi2 = 0;
float a = 0;
float b = 0;
float c = 0;
float d = 0;
float e = 0;
float f = 0;
float te = 0;
float h = 0;
int w = 1;
int z = 0;
int mati = 0;
float min1, max1, min2, max2, volt;
float caltempC, calhumidity, tempC, humidity;
float fval4, fval5, fval6;
float Vout = 0.00;
float Vin = 0.00;
bool portalRunning = false;
bool startAP = false;
//CHAR
extern unsigned char cloud[];
extern unsigned char wifi_icon[];
extern unsigned char thunder[];
extern unsigned char au[];
extern unsigned char sad[];
extern unsigned char happy[];
extern unsigned char suhu[];
extern unsigned char humi[];
extern unsigned char up[];
extern unsigned char down[];
extern unsigned char bar[];
extern unsigned char charge[];
extern unsigned char empty[];
extern unsigned char warning[];
#define MTU_Size 2 * 1460  // this size seems to work best

// file sending from SD card
byte clientBuf[MTU_Size];
int clientCount = 0;
int progress = 0;
int zenklas = 0;
int zenklas2 = 0;
char node_ID[] = "RND";

void setup() {
  Serial.begin(115200);
  Create_Data();
  tft.begin();
  //tft.setBacklight(128);
  tft.setRotation(0);
  tft.fillScreen(ILI9341_WHITE);
  tft.drawBitmap(5, 110, cloud, 231, 75, ILI9341_BLACK);
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 10);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Please connect a WiFi!");
  tft.println(" ");
  wifiManager.setConfigPortalTimeout(300);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Open WiFi Portal for 5 Minutes");
  tft.println(" ");

  if (wifiManager.autoConnect(node_ID)) {
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(1);
    tft.print("Connected to ");
    tft.println(WiFi.SSID());
    tft.println(" ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    tft.setTextColor(ILI9341_CYAN);
    tft.setTextSize(1);
    tft.print("IP address: ");
    tft.println(WiFi.localIP());
    tft.println(" ");
    geturl();
  }

  if (WiFi.status() != WL_CONNECTED) {
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.print("No Connection");
    tft.println(" ");
    tft.println("Device Restarting after 5 Second");
    delay(5000);
    ESP.restart();
  }
  //check komponen
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Firmware Version 1.0.7");
  tft.println(" ");
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println("Checking Components...");
  tft.println(" ");
  bool status;
  sensor.begin();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println("Sensor OK!");
  tft.println(" ");
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(1);
  tft.println("Uploading previous file...");
  tft.println(" ");
  sendfile();
  if (!fatfs.remove("datalog.txt")) {
    Serial.println("Error, couldn't delete datalog.txt file!");
  } else {
    tft.setTextColor(ILI9341_CYAN);
    tft.setTextSize(1);
    tft.println("Done Uploading");
    tft.println(" ");
  }
  Serial.println("connected");
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println("Calibrating.. Please WAIT!");
  tft.println(" ");
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println("STARTING...");
  delay(1000);
  tft.fillScreen(ILI9341_WHITE);
}

void loop() {
  unsigned long currentMillis = millis();        // store the current time
  if (currentMillis - previousMillis >= 3000) {  // check if 1000ms passed
    previousMillis = currentMillis;              // save the last time you blinked the LED
    long rssi = WiFi.RSSI();
    Serial.print("RSSI: ");
    Serial.println(rssi);
    qualwifi = 2 * (rssi + 100);
    Serial.println(w);
    if (WiFi.status() != WL_CONNECTED) {
      tft.drawBitmap(170, 5, wifi_icon, 20, 15, ILI9341_BLACK);
      tft.drawBitmap(177, 5, warning, 7, 16, ILI9341_RED);
      wifiManager.setConnectTimeout(1);
      wifiManager.setConnectRetries(1);
      if (wifiManager.getWiFiIsSaved()) {
        wifiManager.setConfigPortalBlocking(false);
        //wifiManager.setEnableConfigPortal(false);
        wifiManager.autoConnect(node_ID);
      } else {
        wifiManager.autoConnect(node_ID);
      }
      offsite1 = offsite1;
      offsite2 = offsite2;
      lasttime = lasttime;
      if (mati == (interval / 2)) {
        savedata();
        mati = -1;
      }
      Serial.println(mati);
      w = 0;
      mati++;
    } else {
      if (w == 0) {
        geturl();
        Serial.println(lasttime);
        sendfile();
        z = 0;
        if (!fatfs.remove("datalog.txt")) {
          Serial.println("Error, couldn't delete datalog.txt file!");
        }
        Serial.println("Deleted file!");
        delay(2000);
        w = 1;
        mati = 0;
        tft.drawBitmap(177, 5, warning, 7, 16, ILI9341_WHITE);
      }
      sendTofirebase(caltempC, calhumidity, tempC, humidity, link);
      getTimeStamp();
      yield();
    }
  }
  if (currentMillis - previousMillis1 >= 2000) {
    previousMillis1 = currentMillis;
    readSensor();
    testText(caltempC, calhumidity, caltempcclear);
  }
  MDNS.update();
  doWiFiManager();
}
void readSensor() {
  tempC = float(sensor.readTemperature());
  humidity = float(sensor.readHumidity());
  Serial.print("tempC=");
  Serial.println(tempC);
  Serial.print("hum=");
  Serial.println(humidity);
  te = (a * tempC) + (b * (pow(tempC, 2))) + c;
  Serial.println(t);
  h = (d * humidity) + (e * (pow(humidity, 2))) + f;
  caltempC = (tempC + te) * offsite1;
  if (caltempC > max1) {
    tft.drawBitmap(212, 93, up, 17, 17, ILI9341_RED);
    tft.drawBitmap(212, 120, down, 17, 17, ILI9341_BLACK);
    tft.drawBitmap(155, 5, warning, 7, 16, ILI9341_ORANGE);
  } else if (caltempC < min1) {
    tft.drawBitmap(212, 120, down, 17, 17, ILI9341_RED);
    tft.drawBitmap(212, 93, up, 17, 17, ILI9341_BLACK);
    tft.drawBitmap(155, 5, warning, 7, 16, ILI9341_ORANGE);
  } else {
    tft.drawBitmap(155, 5, warning, 7, 16, ILI9341_BLACK);
    tft.drawBitmap(212, 120, down, 17, 17, ILI9341_BLACK);
    tft.drawBitmap(212, 93, up, 17, 17, ILI9341_BLACK);
  }
  calhumidity = (humidity + h) * offsite2;
  if (calhumidity > max2) {
    tft.drawBitmap(212, 195, up, 17, 17, ILI9341_RED);
    tft.drawBitmap(212, 222, down, 17, 17, ILI9341_BLACK);
    tft.drawBitmap(155, 5, warning, 7, 16, ILI9341_ORANGE);
  } else if (calhumidity < min2) {
    tft.drawBitmap(212, 222, down, 17, 17, ILI9341_RED);
    tft.drawBitmap(212, 195, up, 17, 17, ILI9341_BLACK);
    tft.drawBitmap(155, 5, warning, 7, 16, ILI9341_ORANGE);
  } else {
    tft.drawBitmap(155, 5, warning, 7, 16, ILI9341_BLACK);
    tft.drawBitmap(212, 222, down, 17, 17, ILI9341_BLACK);
    tft.drawBitmap(212, 195, up, 17, 17, ILI9341_BLACK);
  }
  currentValue = 0;
  for (int i = 0; i < 10; i++) {
    currentValue += analogRead(bat);
    delay(10);
  }
  volt = (currentValue / 10);
  Serial.print("volt=");
  Serial.println(volt);
  //RESISTOR 1K & 220
  if (volt > 870) {
    if (q == 0) {
      tft.drawBitmap(198, 5, empty, 31, 15, ILI9341_BLACK);
      tft.drawBitmap(200, 7, bar, 9, 11, ILI9341_BLACK);
      tft.drawBitmap(209, 7, bar, 9, 11, ILI9341_BLACK);
      tft.drawBitmap(215, 7, bar, 9, 11, ILI9341_BLACK);
      q = 1;
    }
    tft.drawBitmap(204, 5, charge, 15, 17, ILI9341_YELLOW);
  } else if (volt <= 870 && volt > 760) {
    tft.drawBitmap(198, 5, empty, 31, 15, ILI9341_BLACK);
    tft.drawBitmap(200, 7, bar, 9, 11, ILI9341_BLACK);
    tft.drawBitmap(209, 7, bar, 9, 11, ILI9341_BLACK);
    tft.drawBitmap(215, 7, bar, 9, 11, ILI9341_BLACK);
    q = 0;
  } else if (volt <= 760 && volt > 730) {
    tft.drawBitmap(198, 5, empty, 31, 15, ILI9341_BLACK);
    tft.drawBitmap(200, 7, bar, 9, 11, ILI9341_BLACK);
    tft.drawBitmap(209, 7, bar, 9, 11, ILI9341_BLACK);
    tft.drawBitmap(215, 7, bar, 9, 11, ILI9341_WHITE);
    q = 0;
  } else if (volt <= 730) {
    tft.drawBitmap(198, 5, empty, 31, 15, ILI9341_BLACK);
    tft.drawBitmap(200, 7, bar, 9, 11, ILI9341_RED);
    tft.drawBitmap(209, 7, bar, 9, 11, ILI9341_WHITE);
    tft.drawBitmap(215, 7, bar, 9, 11, ILI9341_WHITE);
    q = 0;
  } else {
    tft.drawBitmap(198, 5, empty, 31, 15, ILI9341_RED);
    tft.drawBitmap(200, 7, bar, 9, 11, ILI9341_WHITE);
    tft.drawBitmap(209, 7, bar, 9, 11, ILI9341_WHITE);
    tft.drawBitmap(215, 7, bar, 9, 11, ILI9341_WHITE);
    q = 0;
  }
}
void Create_Data() {
  if (flash.begin()) {
    Serial.println(F("Device finded and supported!"));
  } else {
    Serial.println(F("Problem to discover and configure device, check wiring also!"));
  }
  Serial.print("JEDEC ID: ");
  Serial.println(flash.getJEDECID(), HEX);
  Serial.print("Flash size: ");
  Serial.println(flash.size());
  Serial.flush();
  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    Serial.println("Was the flash chip formatted with the SdFat_format example?");
    while (1) yield();
  }
  Serial.println("Mounted filesystem!");
  Serial.println();
}

void savedata() {
  String dataString = "";
  if (z == 0) {
    dataString += String(lasttime);
    dataString += "%";
    Serial.print("Last Time: ");
    Serial.println(lasttime);
    z++;
  }
  dataString += String(caltempC);
  dataString += ",";
  dataString += String(calhumidity);
  dataString += '#';

  File32 dataFile = fatfs.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.print(dataString);
    dataFile.close();
    Serial.println(dataString);
  }

  else {
    Serial.println("error opening datalog.txt");
  }
}

void sendTofirebase(float caltempC, float calhumidity, float tempC, float humidity, String link) {
  WiFiClient client;
  HTTPClient http;
  String load = "{}";
  http.begin(client, link);
  getTimeStamp();
  String jsonStr = "";
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"email\":\"" + email + "\",\"password\":\"" + pass + "\", \"userId\":\"" + userId + "\",\"idDevice\":\"" + idDevice + "\",\"value\":{\"data1\":" + caltempC + ",\"data2\":" + calhumidity + ",\"wifi\":" + qualwifi + "}}");
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode != 200) {
    tft.drawBitmap(170, 5, wifi_icon, 20, 15, ILI9341_PURPLE);
  } else {
    load = http.getString();
    Serial.print(load);
    DynamicJsonDocument doc(1024);
    String input = load;
    deserializeJson(doc, input);
    JsonObject obj = doc.as<JsonObject>();
    lasttime = obj["result"]["newresult"]["timestamp"].as<String>();
    offsite1 = obj["result"]["offsite"]["offsite_value_data1"].as<float>();
    offsite2 = obj["result"]["offsite"]["offsite_value_data2"].as<float>();
    min1 = obj["result"]["min"]["min_data1"].as<float>();
    max1 = obj["result"]["max"]["max_data1"].as<float>();
    min2 = obj["result"]["min"]["min_data2"].as<float>();
    max2 = obj["result"]["max"]["max_data2"].as<float>();
    name = obj["result"]["name"].as<String>();
    lasttime = obj["result"]["newresult"]["timestamp"].as<String>();
    Serial.println(offsite1);
    Serial.println(offsite2);
    Serial.println(lasttime);
    Serial.println(name);
    if (obj["result"]["restart"].as<int>() == 1) {
      ESP.restart();
    }
    if (obj["result"]["reset"].as<int>() == 1) {
      wifiManager.resetSettings();
      ESP.restart();
    }
    if (qualwifi < 60) {
      tft.drawBitmap(170, 5, wifi_icon, 20, 15, ILI9341_RED);
    } else if (qualwifi >= 70) {
      tft.drawBitmap(170, 5, wifi_icon, 20, 15, ILI9341_YELLOW);
    } else {
      tft.drawBitmap(170, 5, wifi_icon, 20, 15, ILI9341_GREEN);
    }
    http.end();
  }
}

void testText(float caltempC, float calhumidity, float caltempcclear) {
  tft.setCursor(10, 300);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println(name);

  tft.setFont(&SevenSegment50pt7b);
  tft.setCursor(43, 148);
  tft.setTextSize(1);
  Data1.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  Data1.print(caltempC, 1);

  //draw7Number(caltempC, 85, 75, 4, ILI9341_BLACK, ILI9341_WHITE, 2);  // (kata,X,Y,ukuran,warna,background,side)
  // char onesDigit = '0' + (caltempC % 10);
  // draw7Number(onesDigit, 150, 100, 1, ILI9341_WHITE, ILI9341_BLACK, 1);
  tft.drawBitmap(13, 85, suhu, 32, 52, ILI9341_RED);
  tft.setCursor(75, 250);
  tft.setTextSize(1);
  Data2.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  Data2.print(calhumidity, 0);
  //draw7Number(calhumidity, 85, 180, 4, ILI9341_BLACK, ILI9341_WHITE, 2);  // (kata,X,Y,ukuran,warna,background,side)
  tft.drawBitmap(10, 185, humi, 38, 52, ILI9341_BLUE);
  tft.setFont();
}

void getTimeStamp() {
  formattedDate = lasttime;
  Serial.println(formattedDate);
  // Extract date
  int splitT = formattedDate.indexOf("T");
  jam = String(formattedDate.substring(splitT + 1, formattedDate.length() - 9));
  waktu = jam + "     ";
  Serial.print("DATE: ");
  Serial.println(waktu);
  tft.setCursor(10, 5);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.print(waktu);
}

void geturl() {
  WiFiClient client;
  HTTPClient http;
  String serverPath = serverName + "/" + idDevice;
  http.begin(client, serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
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
    lasttime = obj["time"].as<String>();
    koreksi1 = obj["correction"]["correction_data1"].as<float>();
    koreksi2 = obj["correction"]["correction_data2"].as<float>();
    interval = obj["time_interval"].as<int>();
    a = obj["regression"]["data1"]["a"].as<float>();
    b = obj["regression"]["data1"]["b"].as<float>();
    c = obj["regression"]["data1"]["c"].as<float>();
    d = obj["regression"]["data2"]["a"].as<float>();
    e = obj["regression"]["data2"]["b"].as<float>();
    f = obj["regression"]["data2"]["c"].as<float>();
    name = obj["name"].as<String>();
    Serial.println(name);
    Serial.println(offsite1);
    Serial.println(offsite2);
    //Serial.println(interval);
    Serial.print("urlGet ");
    Serial.println(link);
    Serial.println("connected");

  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.println("Get URL FAILED");
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.println("Restarting !!");
    //ESP.restart();
  }
  http.end();
}

void sendfile() {
  postFileContent("datalog.txt");  // Read file data_log.txt in Root directory
  delay(1);
  client.stop();
}

void postFileContent(const char* path) {
  Serial.print(F(" -- Reading entries from file = "));
  Serial.println(path);
  if (!fatfs.exists(path)) {
    Serial.println("ERROR: The required file does not exist.");
    return;
  }
  File32 dataFile = fatfs.open(path);  // FILE_READ is default so not realy needed but if you like to use this technique for e.g. write you need FILE_WRITE
  if (!dataFile) {
    Serial.println("Failed to open file for reading");
    return;
  }
  char c;
  uint8_t i = 0;


  WiFiClient client;

  if (client.connect("diawan.io", 80)) {

    boolean is_stop = false;
    String lasttime_string = "";
    while (dataFile.available() && !is_stop) {

      if (dataFile.position() < dataFile.size()) {
        c = dataFile.read();
        if (c == '%') {
          is_stop = true;
        }

        if (!is_stop && c != '%') {
          lasttime_string = lasttime_string + c;
        }
      }
    }

    lasttime_string.replace("+", "#");

    Serial.print("lasttime=");
    Serial.println(lasttime_string);
    String data = "&userId=" + String(userId) + "&idDevice=" + String(idDevice) + "&lasttimestamp=" + String(lasttime_string) + "&data=";
    Serial.println("Posting sensor data...");
    client.print("POST ");
    client.print("/api/upload-json");
    client.println(" HTTP/1.1");
    client.println("Host: diawan.io");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");

    int length = data.length();

    client.print("Content-Length: ");
    client.println((dataFile.size() - lasttime_string.length() - 1 - 2) + length);
    Serial.print("length: ");
    Serial.println(String(dataFile.size() - lasttime_string.length() - 1 + length));
    client.println();
    client.print(data);
    while (dataFile.available()) {

      if (dataFile.position() < dataFile.size() && (dataFile.position() > (lasttime_string.length() - 1))) {
        if (dataFile.position() < dataFile.size() - 2) {
          c = dataFile.read();

          Serial.print("c=");
          Serial.println(c);
          client.print(c);
          //data=data+c;
        } else {
          c = dataFile.read();
          if (c != ' ') {
            if (c != '#') {
              Serial.print("c=");
              Serial.print(c);
              client.print(c);
              //data=data+c;
            }
          }
        }
      }
      if (dataFile.position() == dataFile.size() - 1 && (dataFile.position() > (lasttime_string.length() - 1))) {
        client.println();
      }
    }
    Serial.print(data);
    Serial.println();
    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();
    Serial.println("Client disonnected");
    dataFile.close();


  } else {
    Serial.println("Connection failed");
  }
  Serial.println(F("DONE Reading"));
}
void doWiFiManager() {
  // is auto timeout portal running
  if (portalRunning) {
    wifiManager.process();  // do processing

    // check for timeout
    if ((millis() - startTime) > (timeout * 1000)) {
      Serial.println("portaltimeout");
      portalRunning = false;
      if (startAP) {
        wifiManager.stopConfigPortal();
      } else {
        wifiManager.stopWebPortal();
      }
    }
  }
  // is configuration portal requested?
  if (!portalRunning) {
    if (startAP) {
      Serial.println("Button Pressed, Starting Config Portal");
      wifiManager.setConfigPortalBlocking(false);
      wifiManager.startConfigPortal(node_ID);
    } else {
      Serial.println("Button Pressed, Starting Web Portal");
      wifiManager.startWebPortal();
    }
    portalRunning = true;
    startTime = millis();
  }
}" HTTP/1.1");
    client.println("Host: diawan.io");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");

    int length = data.length();

    client.print("Content-Length: ");
    client.println((dataFile.size() - lasttime_string.length() - 1 - 2) + length);
    Serial.print("length: ");
    Serial.println(String(dataFile.size() - lasttime_string.length() - 1 + length));
    client.println();
    client.print(data);
    while (dataFile.available()) {

      if (dataFile.position() < dataFile.size() && (dataFile.position() > (lasttime_string.length() - 1))) {
        if (dataFile.position() < dataFile.size() - 2) {
          c = dataFile.read();

          Serial.print("c=");
          Serial.println(c);
          client.print(c);
          //data=data+c;
        } else {
          c = dataFile.read();
          if (c != ' ') {
            if (c != '#') {
              Serial.print("c=");
              Serial.print(c);
              client.print(c);
              //data=data+c;
            }
          }
        }
      }
      if (dataFile.position() == dataFile.size() - 1 && (dataFile.position() > (lasttime_string.length() - 1))) {
        client.println();
      }
    }
    Serial.print(data);
    Serial.println();
    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();
    Serial.println("Client disonnected");
    dataFile.close();


  } else {
    Serial.println("Connection failed");
  }
  Serial.println(F("DONE Reading"));
}
void doWiFiManager() {
  // is auto timeout portal running
  if (portalRunning) {
    wifiManager.process();  // do processing

    // check for timeout
    if ((millis() - startTime) > (timeout * 1000)) {
      Serial.println("portaltimeout");
      portalRunning = false;
      if (startAP) {
        wifiManager.stopConfigPortal();
      } else {
        wifiManager.stopWebPortal();
      }
    }
  }
  // is configuration portal requested?
  if (!portalRunning) {
    if (startAP) {
      Serial.println("Button Pressed, Starting Config Portal");
      wifiManager.setConfigPortalBlocking(false);
      wifiManager.startConfigPortal(node_ID);
    } else {
      Serial.println("Button Pressed, Starting Web Portal");
      wifiManager.startWebPortal();
    }
    portalRunning = true;
    startTime = millis();
  }
}

void draw7Number(int n, unsigned int xLoc, unsigned int yLoc, char cS, unsigned int fC, unsigned int bC, char nD) {
  unsigned int num = abs(n), i, s, t, w, col, h, a, b, si = 0, j = 1, d = 0;
  unsigned int S2 = 5 * cS;                                                                                                                                     // width of horizontal segments   5 times the cS
  unsigned int S3 = 2 * cS;                                                                                                                                     // thickness of a segment 2 times the cs
  unsigned int S4 = 7 * cS;                                                                                                                                     // height of vertical segments 7 times the cS
  unsigned int x1 = cS + 1;                                                                                                                                     // starting x location of horizontal segments
  unsigned int x2 = S3 + S2 + 1;                                                                                                                                // starting x location of right side segments
  unsigned int y1 = yLoc + x1;                                                                                                                                  // starting y location of top side segments
  unsigned int y3 = yLoc + S3 + S4 + 1;                                                                                                                         // starting y location of bottom side segments
  unsigned int seg[7][3] = { { x1, yLoc, 1 }, { x2, y1, 0 }, { x2, y3 + x1, 0 }, { x1, (2 * y3) - yLoc, 1 }, { 0, y3 + x1, 0 }, { 0, y1, 0 }, { x1, y3, 1 } };  // actual x,y locations of all 7 segments with direction
  unsigned char nums[12] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x00, 0x40 };                                                          // segment defintions for all 10 numbers plus blank and minus sign
  unsigned char c, cnt;

  c = abs(cS);  // get character size between 1 and 10 ignoring sign
  if (c > 10) c = 10;
  if (c < 1) c = 1;

  cnt = abs(nD);  // get number of digits between 1 and 10 ignoring sign
  if (cnt > 10) cnt = 10;
  if (cnt < 1) cnt = 1;

  xLoc += (cnt - 1) * (d = S2 + (3 * S3) + 2);  // set starting x at last digit location

  while (cnt > 0) {  // for cnt number of places

    --cnt;

    if (num > 9) i = num % 10;        //get the last digit
    else if (!cnt && n < 0) i = 11;   //show
