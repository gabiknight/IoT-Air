/**
 * @file wifi.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef WIFI_HPP
#define WIFI_HPP

#include <ESP8266WiFi.h>
#include "common.h"
#include "asyncwebserver.hpp"

/**
 * @brief Busca redes wifi disponibles y las guarda en una lista enlazada.
 * 
 */
void wifi_scan();

/**
 * @brief Elimina la lista enlazada de redes wifi.
 * 
 */
void delete_ssid_list();

/**
 * @brief Guarda la red wifi en la lista enlazada.
 * 
 * @param name 
 */
void save_ssid_in_list(char *name);

void initWiFi();

String web_callback(String var);
void startWifiManagerWeb();
void stopWifiManagerWeb();

#endif /* WIFI_HPP */