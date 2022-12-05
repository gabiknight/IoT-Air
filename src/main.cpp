
// #include <header.h>
#include <SoftwareSerial.h>
#include "common.h"
#include "filesystem.hpp"
#include "wifi.hpp"
#include <ESP8266httpUpdate.h>
#include "mqtt.hpp"
#include "sensor.hpp"
//! EN LIBRERIA DE MULTICHANNEL CAMBIAR POR Wire.begin(D4, D3);

#define MEASURING_PERIOD_MS 10000

parametros_t parametros;

extern ambi_t mAmbi;

bool web_activada = false;
long previousMillis;

#ifdef PLANTPHENOL
SoftwareSerial Serial_apm;
#endif

#ifdef HONEYWELL
SoftwareSerial Serial_hpm;
#endif

void crearJson();

void setup()
{
  pinMode(D8, OUTPUT);

  // digitalWrite(D8, HIGH);
  //  Serial port for debugging purposes
  Serial.begin(9600);
  #ifdef HONEYWELL
  Serial_hpm.begin(9600, SWSERIAL_8N1, D7, D6, false); // Rx Tx
  #endif

  #ifdef PLANTPHENOL
  Serial_apm.begin(9600, SWSERIAL_8N1, D5, D8, false);
  #endif

  if (!Serial_hpm)
  { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid SoftwareSerial pin configuration, check config");
    while (1)
    { // Don't continue with invalid configuration
      delay(1000);
    }
  }

  ESPhttpUpdate.rebootOnUpdate(false);

  inicializar_archivos(&parametros);

  PRINTF("SSID: %s\n", parametros.ssid);
  PRINTF("PASS: %s\n", parametros.pass);

  initWiFi();
  set_MQTT_parameters();

  gas_sensor_init();

  while (Serial_hpm.available() > 0)
  {
    Serial_hpm.read();
  }

  pm_sensor_init();

  ht_sensor_init();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    mqtt_reconnect();

    mqtt_loop();

    gas_sensor_read();

    pm_sensor_read();

    ht_sensor_read();

#ifdef DEBUG
    sensor_print();
#endif
    crearJson();

    previousMillis = millis();
    while (millis() - previousMillis < 60000)
    {
      mqtt_reconnect();

      mqtt_loop();
    }
    PRINTF("A mimir...\n");
    ESP.deepSleep(300 * 1000000, WAKE_RF_DEFAULT);
  }
  else
  {
    initWiFi();
  }
}

void crearJson()
{
  StaticJsonDocument<384> doc;

  char json_values[384];

// 20220523203501
#ifdef AMBIMATE
  if (mAmbi.temperatura_am != 0)
    doc["temp"] = String(mAmbi.temperatura_am, 2);
  if (mAmbi.humedad_am != 0)
    doc["hum"] = String(mAmbi.humedad_am, 2);
#endif

#ifdef SHT_SENSOR
  if (mAmbi.temperatura_sht != 0)
    doc["temp"] = String(mAmbi.temperatura_sht, 2);
  if (mAmbi.humedad_sht != 0)
    doc["hum"] = String(mAmbi.humedad_sht, 2);
#endif

#ifdef MULTICHANNEL
  if (mAmbi.co_ppm != 0)
    doc["co"] = mAmbi.co_ppm;
  if (mAmbi.no2_ppm != 0)
    doc["no2"] = String(mAmbi.no2_ppm, 2);
  if (mAmbi.nh3_ppm != 0)
    doc["nh3"] = mAmbi.nh3_ppm;
#endif

#ifdef AMBIMATE
  doc["voc"] = mAmbi.voc_ppm;
  if (mAmbi.co2_ppm != 0)
    doc["co2"] = mAmbi.co2_ppm;
#endif

#ifdef HONEYWELL
  if (mAmbi.pm2_5 != 0)
    doc["pm25"] = mAmbi.pm2_5;
  if (mAmbi.pm10 != 0)
    doc["pm10"] = mAmbi.pm10;
#endif

#ifdef PLANTPHENOL
  if (mAmbi.pm2_5_plant != 0)
    doc["pm25_p"] = mAmbi.pm2_5_plant;
  if (mAmbi.pm10_plant != 0)
    doc["pm10_p"] = mAmbi.pm10_plant;
#endif

  serializeJson(doc, json_values);

  mqtt_print(json_values);
}