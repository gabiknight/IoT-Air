/**
 * @file filesystem.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <LittleFS.h>
#include "common.h"



/**
 * @brief Lee archivos de la memoria flash.
 * 
 * @param parametros 
 */
void inicializar_archivos(parametros_t *parametros);
void writeFile(fs::FS &fs, const char *path, const char *message);
void save_ssid(const char *ssid);
void save_pass(const char *pass);
void save_id(const char *id);
void save_mqtt_server(const char *mqtt_server);
void save_mqtt_port(const char *mqtt_port);
void save_mqtt_user(const char *mqtt_user);
void save_mqtt_pass(const char *mqtt_pass);

#endif /* FILESYSTEM_HPP */