//#include <header.h>
#include "asyncwebserver.hpp"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

extern parametros_t parametros;

// extern String ssid, pass, ID;

// Search for parameter in HTTP POST request
#define PARAM_INPUT_1 "ssid"
#define PARAM_INPUT_2 "pass"
#define PARAM_INPUT_3 "id"
#define PARAM_INPUT_4 "mqtt_ip"
#define PARAM_INPUT_5 "mqtt_port"
#define PARAM_INPUT_6 "urlrep"
#define PARAM_INPUT_7 "rweb"
#define PARAM_INPUT_8 "tipo"
#define PARAM_INPUT_9 "mqtt_usr"
#define PARAM_INPUT_10 "mqtt_pass"

// extern const char *ssidPath;
// extern const char *passPath;
// extern const char *MQTTportPath;
// extern const char *MQTTbrokerPath;
// extern char *mqtt_broker;
// extern int mqtt_port;

void web_wifimanager()
{

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/wifimanager.html", String(), false, web_callback); });

  server.serveStatic("/", LittleFS, "/");

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
            {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            if(p->value() != "")
            {              
            strcpy(parametros.ssid, p->value().c_str());
            PRINTF("SSID set to: %s\n", parametros.ssid);                   
            save_ssid(parametros.ssid);
            }
            // Write file to save value
            save_ssid(parametros.ssid);
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            strcpy(parametros.pass, p->value().c_str());
            PRINTF("Password set to: %s\n", parametros.pass);
            // Write file to save value
            save_pass(parametros.pass);
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_3) {
            char newID[7];
            strcpy(newID, p->value().c_str());            
            if (strcmp(parametros.id, newID) != 0)
            {
              Serial.printf("#%s a #%s\r\n", parametros.id, newID);
              strcpy(parametros.id, newID);
              PRINTF("ID set to: %s\n", parametros.id);
              // Write file to save value
              save_id(parametros.id);
            }
          }
          if (p->name() == PARAM_INPUT_4) {
            strcpy(parametros.mqtt_server, p->value().c_str());
            PRINTF("MQTT ip set to: %s\n", parametros.mqtt_server);
            // Write file to save value
            save_mqtt_server(parametros.mqtt_server);
          }
          if (p->name() == PARAM_INPUT_5) {
            strcpy(parametros.mqtt_port, p->value().c_str());
            PRINTF("MQTT port set to: %s\n", parametros.mqtt_port);
            // Write file to save value
            save_mqtt_port(parametros.mqtt_port);
          }
          if (p->name() == PARAM_INPUT_9) {
            strcpy(parametros.mqtt_user, p->value().c_str());
            PRINTF("MQTT usr set to: %s\n", parametros.mqtt_port);
            // Write file to save value
            save_mqtt_user(parametros.mqtt_user);
          }
          if (p->name() == PARAM_INPUT_10) {
            strcpy(parametros.mqtt_pass, p->value().c_str());
            PRINTF("MQTT pass set to: %s\n", parametros.mqtt_port);
            // Write file to save value
            save_mqtt_pass(parametros.mqtt_pass);
          }
          if (p->name() == PARAM_INPUT_6) {
            strcpy(parametros.urlrep, p->value().c_str());
            PRINTF("URLRep set to: %s\n", parametros.urlrep);
            // Write file to save value
            //Serial.printf("#%s URLREP %s\r\n", parametros.id, parametros.urlrep);
            parametros.sms_config = true;
            
          }
          if (p->name() == PARAM_INPUT_7) {
            if(p->value() == "on")
            {
              parametros.rweb = true;
              PRINTF("RWEB activado.\r\n");
              //Serial.printf("#%s RWEB ON\r\n", parametros.id);
              parametros.sms_config = true;
            }
            else
            {
              parametros.rweb = false;
              PRINTF("RWEB desactivado.\r\n");
              //Serial.printf("#%s RWEB OFF\r\n", parametros.id);
              parametros.sms_config = true;
            }
          }
          if (p->name() == PARAM_INPUT_8) {
            if(p->value() == "com")
            {
              parametros.comunitaria = true;
              PRINTF("Tipo comunitaria.\r\n");
              //Serial.printf("#%s RWEB ON\r\n", parametros.id);
              parametros.sms_config = true;
            }
            else
            {
              parametros.comunitaria = false;
              PRINTF("Tipo sensores.\r\n");
              //Serial.printf("#%s RWEB OFF\r\n", parametros.id);
              parametros.sms_config = true;
            }
          }
          
         
        }
      }     

      //request->send(200, "text/plain", "Alarma configurada."); 
      //stopWifiManagerWeb(); 
    });
  server.begin();
}

void stopServer()
{
  server.end();
}