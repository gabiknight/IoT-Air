
#include "filesystem.hpp"

// File paths to save input values permanently
const char *ssidPath = "/ssid.txt";
const char *passPath = "/pass.txt";
const char *idPath = "/id.txt";
const char *MQTTportPath = "/port_mqtt.txt";
const char *MQTTbrokerPath = "/servidor_mqtt.txt";
const char *MQTTuserPath = "/usuario_mqtt.txt";
const char *MQTTpassPath = "/pass_mqtt.txt";


/**
 * @brief Initialize LittleFS
 *
 * @param fs
 */
void initFS(fs::FS &fs)
{
  if (!fs.begin())
  {
    PRINTF("An error has occurred while mounting LittleFS\r\n");
  }
  else
  {
    PRINTF("LittleFS mounted successfully\r\n");
  }
}

/**
 * @brief Read File from LittleFS
 *
 * @param fs
 * @param path
 * @return String
 */
String readFile(fs::FS &fs, const char *path)
{
  PRINTF("Reading file: %s", path);

  File file = fs.open(path, "r");
  if (!file)
  {
    PRINTF("- failed to open file for reading\r\n");
    return String();
  }

  String fileContent;
  while (file.available())
  {
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

/**
 * @brief Write file to LittleFS
 *
 * @param fs
 * @param path
 * @param message
 */
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  PRINTF("Writing file: %s", path);

  File file = fs.open(path, "w");
  if (!file)
  {
    PRINTF("- failed to open file for writing\r\n");
    return;
  }
  if (file.print(message))
  {
    PRINTF("- file written\r\n");
  }
  else
  {
    PRINTF("- frite failed\r\n");
  }
  file.close();
}


void inicializar_archivos(parametros_t *parametros)
{
  initFS(LittleFS);

  strcpy(parametros->ssid, readFile(LittleFS, ssidPath).c_str());
  PRINTF("ssid: %s\r", parametros->ssid);
  strcpy(parametros->pass, readFile(LittleFS, passPath).c_str());
  PRINTF("pass: %s\r", parametros->pass);
  strcpy(parametros->id, readFile(LittleFS, idPath).c_str());
  PRINTF("id: %s\r", parametros->id);
  strcpy(parametros->mqtt_server, readFile(LittleFS, MQTTbrokerPath).c_str());
  if(parametros->mqtt_server[0] == 0)
  {
    strcpy(parametros->mqtt_server, "alvear.reclamos311.com.ar");
  }
  PRINTF("mqtt_server: %s\r", parametros->mqtt_server);
  strcpy(parametros->mqtt_port, readFile(LittleFS, MQTTportPath).c_str());
  if(parametros->mqtt_port[0] == 0)
  {
    strcpy(parametros->mqtt_port, "1883");
  }
  PRINTF("mqtt_port: %s\r", parametros->mqtt_port);
  strcpy(parametros->mqtt_user, readFile(LittleFS, MQTTuserPath).c_str());
  if(parametros->mqtt_user[0] == 0)
  {
    strcpy(parametros->mqtt_user, "rlink");
  }
  PRINTF("mqtt_user: %s\r", parametros->mqtt_user);
  strcpy(parametros->mqtt_pass, readFile(LittleFS, MQTTpassPath).c_str());
  if(parametros->mqtt_pass[0] == 0)
  {
    strcpy(parametros->mqtt_pass, "7ujmU(HB");
  }
  PRINTF("mqtt_pass: %s\r", parametros->mqtt_pass);
}

void save_ssid(const char *ssid)
{
  writeFile(LittleFS, ssidPath, ssid);
}

void save_pass(const char *pass)
{
  writeFile(LittleFS, passPath, pass);
}

void save_id(const char *id)
{
  writeFile(LittleFS, idPath, id);
}

void save_mqtt_server(const char *mqtt_server)
{
  writeFile(LittleFS, MQTTbrokerPath, mqtt_server);
}

void save_mqtt_port(const char *mqtt_port)
{
  writeFile(LittleFS, MQTTportPath, mqtt_port);
}

void save_mqtt_user(const char *mqtt_user)
{
  writeFile(LittleFS, MQTTuserPath, mqtt_user);
}

void save_mqtt_pass(const char *mqtt_pass)
{
  writeFile(LittleFS, MQTTpassPath, mqtt_pass);
}

