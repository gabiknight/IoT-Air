/**
 * @file mqtt.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef MQTT_HPP
#define MQTT_HPP

#include <ArduinoJson.h>
#include "wifi.hpp"
#include <ESP8266httpUpdate.h>


void mqtt_loop();
void set_MQTT_parameters();
void mqtt_reconnect();
void callback(char *topic, byte *payload, unsigned int length);
void mqtt_disconnect();
void mqtt_print(const char *payload);
void interpreter(byte *payload);
bool mqtt_connected();

#endif /* MQTT_HPP */