/**
 * @file sensor.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SENSOR_HPP
#define SENSOR_HPP

#define SHT_SENSOR
#define HONEYWELL
#define MULTICHANNEL
//#define AMBIMATE
//#define PLANTPHENOL

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "common.h"


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


void ht_sensor_read();
void ht_sensor_init();
void pm_sensor_read();
void stop_measuring_pm();
void pm_sensor_init();
void gas_sensor_read();
void gas_sensor_init();
void sensor_print();
#endif