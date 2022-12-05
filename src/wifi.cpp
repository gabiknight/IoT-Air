#include <header.h>

extern String ssid, pass;
wifiScanList_t *wifiRoot = NULL;
wifiScanList_t *wifiTail = NULL;

/**
 * @brief Initialize WiFi
 *
 * @return true
 * @return false
 */
bool initWiFi()
{
  if (ssid == "" || pass == "")
  {
    PRINTF("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid.c_str(), pass.c_str());

  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
  {
    PRINTF("Connecting to WiFi...");
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    PRINTF("Failed to connect.");
    return false;
  }

  PRINTF("Local IP: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

void startWifiManagerWeb()
{
  // Connect to Wi-Fi network with SSID and password
  PRINTF("Setting AP (Access Point)");
  // NULL sets an open Access Point
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("gc_sensor3", NULL);

  IPAddress IP = WiFi.softAPIP();
  PRINTF("AP IP address: %s", IP.toString().c_str());

  wifi_scan();

  web_wifimanager();
}

// se ingresa la SSID de una red wifi y la guarda en una fila de estructuras que va desde wifiRoot hasta wifiTail
void save_ssid_in_list(String name)
{
  wifiScanList_t *nodo;                                    // puntero a una estructura
  nodo = (wifiScanList_t *)malloc(sizeof(wifiScanList_t)); // reserva espacio en memoria para estructura
  if (nodo)
  {                                                  // verificar que hay memoria disponible
    name.toCharArray(nodo->name, name.length() + 1); // guarda la SSID en la variable name del arreglo
    nodo->next = NULL;                               // el puntero apunta a NULL
    if (wifiRoot == NULL)
    { // la primera vez hace que root y tail apunten a nodo
      wifiRoot = nodo;
      wifiTail = nodo;
    }
    else
    {
      wifiTail->next = nodo; // modifica next del nodo anterior, ya que tail estaba apuntando al nodo anterior
      wifiTail = nodo;       // tail ahora apunta al nodo actual
    }
  }
}

// recorre la fila de estructuras y extrae los SSID para imprimirlos por pantalla
String read_ssid_from_list(const String &var)
{
  if (var == "LIST")
  {
    String html = "";
    wifiScanList_t *nodo = wifiRoot; // la fila comienza en wifiRoot
    while (nodo != NULL)
    {
      html += "<option value='" + (String)nodo->name + "'>" + (String)nodo->name + "</option>";
      nodo = nodo->next; // cuando llegue a wifitail next apuntará a null, y saldrá del bucle
    }
    delete_ssid_list();
    return html;
  }
  return String();
}

void delete_ssid_list()
{
  String html = "";
  wifiScanList_t *nodo = wifiRoot; // la fila comienza en wifiRoot
  wifiScanList_t *aux;

  while (nodo != NULL)
  {
    aux = nodo->next;
    free(nodo);
    nodo = aux; // cuando llegue a wifitail next apuntará a null, y saldrá del bucle
  }
}

void wifi_scan()
{

  // WiFi.scanNetworks devuelve el número de redes wifi encontradas
  int n = WiFi.scanNetworks();
  if (n <= 0)
  {
    PRINTF("no networks found, detected %i\n", n);
  }
  else
  {
    PRINTF("detected %i\n", n);
    for (int i = 0; i < n; ++i)
    {
      // WiFi.SSID(i) devuelve la SSID de la red encontrada número i
      save_ssid_in_list((String)WiFi.SSID(i)); // guarda las SSID en la estructura wifiScanList_t
      delay(10);
    }
  }
}