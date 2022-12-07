// #include <header.h>
#include "wifi.hpp"

wifiScanList_t *wifiRoot = NULL;
wifiScanList_t *wifiTail = NULL;
extern parametros_t parametros;

void initWiFi()
{
  if (parametros.ssid[0] == '\0')
  {
    PRINTF("No hay SSID guardada, iniciando WifiManager\n");
    startWifiManagerWeb();
    return;
  }

  if (WiFi.getMode() != WIFI_AP_STA)
  {
    WiFi.mode(WIFI_STA);
  }

  WiFi.begin(parametros.ssid, parametros.pass);

  PRINTF("Connecting to WiFi");
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
  {
    PRINTF("...");
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    PRINTF("Failed to connect.\r");
    startWifiManagerWeb();
    return;
  }

  PRINTF("Local IP: %s\r\n", WiFi.localIP().toString().c_str());
}

void startWifiManagerWeb()
{
  char SSID[32];

  PRINTF("Setting AP (Access Point)");

  if (WiFi.getMode() == WIFI_AP_STA)
  {
    return;
  }

  WiFi.mode(WIFI_AP_STA);

  strcpy(SSID, "IoT-Air_");
  strcat(SSID, parametros.id);
  WiFi.softAP(SSID);

  IPAddress IP = WiFi.softAPIP();
  PRINTF("AP IP address: %s", IP.toString().c_str());

  wifi_scan();

  web_wifimanager();
}

void stopWifiManagerWeb()
{
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  stopServer();
}

void save_ssid_in_list(char *name)
{
  wifiScanList_t *nodo;
  nodo = (wifiScanList_t *)malloc(sizeof(wifiScanList_t));
  if (nodo)
  {
    strcpy(nodo->ssid, name);
    nodo->next = NULL;
    if (wifiRoot == NULL)
    {
      wifiRoot = nodo;
      wifiTail = nodo;
    }
    else
    {
      wifiTail->next = nodo;
      wifiTail = nodo;
    }
  }
}

String web_callback(String var)
{
  extern parametros_t parametros;
  char html[1024];

  // wifi_scan();

  if (var == "LIST")
  {
    strcpy(html, "<option value='");
    strcat(html, parametros.ssid);
    strcat(html, "'>");
    strcat(html, parametros.ssid);
    strcat(html, "</option>");

    wifiScanList_t *nodo = wifiRoot;
    while (nodo != NULL)
    {
      strcat(html, "<option value='");
      strcat(html, nodo->ssid);
      strcat(html, "'>");
      strcat(html, nodo->ssid);
      strcat(html, "</option>");
      nodo = nodo->next;
    }
    delete_ssid_list();
    return String(html);
  }
  if (var == "PASS")
  {
    return String(parametros.pass);
  }
  if (var == "ID")
  {
    return String(parametros.id);
  }
  if (var == "MQTT_IP")
  {
    return String(parametros.mqtt_server);
  }
  if (var == "MQTT_PORT")
  {
    return String(parametros.mqtt_port);
  }
  if (var == "MQTT_USR")
  {
    return String(parametros.mqtt_user);
  }
  if (var == "MQTT_PASS")
  {
    return String(parametros.mqtt_pass);
  }
  return String();
}

void delete_ssid_list()
{
  wifiScanList_t *nodo = wifiRoot;
  wifiScanList_t *aux;

  while (nodo != NULL)
  {
    aux = nodo->next;
    free(nodo);
    nodo = aux;
  }
}

void wifi_scan()
{
  char ssid[32];
  int networks_qty = WiFi.scanNetworks();
  if (networks_qty <= 0)
  {
    PRINTF("no networks found, detected %i\n", networks_qty);
  }
  else
  {
    PRINTF("detected %i\n", networks_qty);
    for (int i = 0; i < networks_qty; ++i)
    {
      strcpy(ssid, WiFi.SSID(i).c_str());
      save_ssid_in_list(ssid);
    }
  }
}