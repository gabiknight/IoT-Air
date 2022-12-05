/**
 * @file asyncwebserver.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef ASYNCWEBSERVER_HPP
#define ASYNCWEBSERVER_HPP

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "wifi.hpp"
#include "filesystem.hpp"

void web_wifimanager();
void stopServer();
#endif /* ASYNCWEBSERVER_HPP */
