#ifndef HEADER_H_
#define HEADER_H_

#define VERSION "0.1"
#define DEBUG
#define DEBUG_SENSOR 1
#define MEASURING_PERIOD_MS 10000
#define HONEYWELL
#define AMBIMATE
#define MULTICHANNEL
//#define SHT_SENSOR
//#define PLANTPHENOL

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <NTPClient.h>
#include <SoftwareSerial.h>

#ifdef PLANTPHENOL
#include <PMserial.h>
#endif

#ifdef SHT_SENSOR
#include <SHT2x.h>
#endif

#ifdef HONEYWELL
#include <hpma115S0.h> //el sensor se debe reinicia si se reinicia o duerme el esp
#endif

#ifdef MULTICHANNEL
#include <MutichannelGasSensor.h>
#endif

#ifdef DEBUG
#define PRINTF(...) Serial.printf(__VA_ARGS__)
#define PRINTL(...) Serial.print(__VA_ARGS__)
#define PRINTLN(...) Serial.print(__VA_ARGS__)
#else
#define PRINTF(...)
#define PRINTL(...)
#define PRINTLN(...)
#endif

extern const char *ssidPath;
extern const char *passPath;
extern const char *idPath;
extern const char *typePath;
extern const char *urlReportPath;

/////////////   ESTRUCTURAS ////////

typedef struct wifiScanList
{
  char name[32] = "";
  struct wifiScanList *next;
} wifiScanList_t;

extern wifiScanList_t *wifiRoot, *wifiTail;

typedef struct 
{
  unsigned int estado;
  float temperatura_am;
  float temperatura_sht;
  float humedad_am;
  float humedad_sht;
  unsigned int luminosidad;
  unsigned int audio;
  unsigned int co2_ppm = 0;
  unsigned int voc_ppm = 0;
  unsigned int pir = 0;
  unsigned int pm2_5 = 0;
  unsigned int pm2_5_plant = 0;
  unsigned int pm10 = 0;
  unsigned int pm10_plant = 0;
  unsigned int co_ppm = 0;
  float no2_ppm = 0;
  unsigned int nh3_ppm = 0;
} ambi_t;
extern ambi_t mAmbi;

extern String date;

/////////////   FUNCIONES   ///////////////

//  WIFI
bool initWiFi();
void startWifiManagerWeb();
void save_ssid_in_list(String name);
String read_ssid_from_list(const String &var);
void delete_ssid_list();
void wifi_scan();

//  FILESYSTEM
void initFS(fs::FS &fs);
String readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);

//  ASYNCWEBSERVER
String processor(const String &var);
void async_web_server();
void web_wifimanager();

//  MQTT
void mqtt_loop();
void set_MQTT_parameters();
void mqtt_reconnect();
void configRegistration();
void callback(char *topic, byte *payload, unsigned int length);
void interpreter(byte *payload);
void mqtt_print(const char *payload);

//  GAS SENSOR
void sensor_print();
void gas_sensor_init();
void gas_sensor_read();

//  PM SENSOR
void pm_sensor_init();
void start_measuring_pm();
void stop_measuring_pm();
void pm_sensor_read();

//  TIME
String get_date_ntp();

//  JSON
void crearJson();

//  HT SENSOR
void ht_sensor_init();
void ht_sensor_read();

#endif