#include <header.h>

// Set LED GPIO

// Stores LED state
extern String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

extern boolean restart;

extern String ssid, pass;

// Search for parameter in HTTP POST request
const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "pass";

extern const char *ssidPath;
extern const char *passPath;

// Replaces placeholder with LED state value
String processor(const String &var)
{
  if (var == "STATE")
  {
    if (!digitalRead(2))
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    return ledState;
  }
  return String();
}

void async_web_server()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html", false, processor); });

  server.serveStatic("/", LittleFS, "/");

  // Route to set GPIO state to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      digitalWrite(2, LOW);
      request->send(LittleFS, "/index.html", "text/html", false, processor); });

  // Route to set GPIO state to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      digitalWrite(2, HIGH);
      request->send(LittleFS, "/index.html", "text/html", false, processor); });
  server.begin();
}

void web_wifimanager()
{
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              request->send(LittleFS, "/wifimanager.html", String(), false, read_ssid_from_list);
            }
  );

  server.serveStatic("/", LittleFS, "/");

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
            {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            PRINTF("SSID set to: %s\n", ssid.c_str());
            // Write file to save value
            writeFile(LittleFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            PRINTF("Password set to: %s\n", pass.c_str());
            // Write file to save value
            writeFile(LittleFS, passPath, pass.c_str());
          }
          // // HTTP POST ip value
          // if (p->name() == PARAM_INPUT_3) {
          //   ip = p->value().c_str();
          //   Serial.print("IP Address set to: ");
          //   Serial.println(ip);
          //   // Write file to save value
          //   writeFile(LittleFS, ipPath, ip.c_str());
          // }
          // // HTTP POST gateway value
          // if (p->name() == PARAM_INPUT_4) {
          //   gateway = p->value().c_str();
          //   Serial.print("Gateway set to: ");
          //   Serial.println(gateway);
          //   // Write file to save value
          //   writeFile(LittleFS, gatewayPath, gateway.c_str());
          // }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      restart = true;
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + WiFi.localIP().toString()); });
  server.begin();
}