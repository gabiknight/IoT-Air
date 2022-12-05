/**
 * @file common.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-09-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <Arduino.h>

#define DEBUG
#ifdef DEBUG
#define PRINTF(...) Serial.printf(__VA_ARGS__)
#define PRINTL(...) Serial.print(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define VERSION "1.0.1"

typedef struct parametros_t
{
    char id[7];
    char ssid[32];
    char pass[32];
    char mqtt_server[32];
    char mqtt_port[6];
    char mqtt_user[32];
    char mqtt_pass[32];
    int8_t wifi_signal = 0;
    char urlrep[100];
    bool rweb, comunitaria;
    bool sms_config;
} parametros_t;

typedef struct wifiScanList
{
    char ssid[32] = "";
    struct wifiScanList *next;
} wifiScanList_t;

#endif /* COMMON_H */