#include "common.h"
#include "mqtt.hpp"
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>

// MQTT Broker
extern parametros_t parametros;

char topic_command[] = "sensor/command/";
char topic_status[] = "sensor/status/0000";
WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);
char topic[30];
char aux[50];
// int delayms = 5000; // Tiempo entre consultas del estado

void mqtt_loop()
{
    mqtt_client.loop();
}

void set_MQTT_parameters()
{
    // client.setKeepAlive(1.5 * delayms);     // KeepAlive de conexion MQTT en segundos
    // client.setSocketTimeout(1.5 * delayms); // TimeOut de conexion MQTT en segundos
    mqtt_client.setServer(parametros.mqtt_server, atoi(parametros.mqtt_port));
    mqtt_client.setCallback(callback);
}

bool mqtt_connected()
{
    return mqtt_client.connected();
}

void mqtt_reconnect()
{
    // Loop until we're reconnected
    if (!mqtt_client.connected())
    {
        strcpy(topic_status, "sensor/status/");
        strcat(topic_status, parametros.id);

        PRINTF("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt_client.connect(parametros.id, parametros.mqtt_user, parametros.mqtt_pass)) // ID del cliente
        {
            PRINTF("connected\n");

            strcpy(topic, topic_command);
            strcat(topic, parametros.id);

            mqtt_client.subscribe(topic);

            PRINTF("[%s] Connected to topic %s\n", parametros.id, topic);
        }
        else
        {
            PRINTF("[%s] failed, rc=%d \n", parametros.id, mqtt_client.state());
            delay(5000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    // Serial.print("Message arrived in topic: ");
    // Serial.println(topic);
    // Serial.print("Message:");
    // for (unsigned int i = 0; i < length; i++)
    // {
    //   Serial.print((char)payload[i]);
    // }
    // Serial.println();
    // Serial.println("-----------------------");
    interpreter(payload);
}

void interpreter(byte *payload)
{
    StaticJsonDocument<512> doc;

    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        PRINTF("deserializeJson() failed: %s", error.c_str());
        return;
    }

    if (!doc["id"].isNull())
    {
        const char *id = doc["id"];
        if (strcmp(id, parametros.id) != 0)
        {
            Serial.printf("#%s a #%s\r\n", parametros.id, id);
            strcpy(parametros.id, id);
            save_id(parametros.id);
            mqtt_disconnect();
            mqtt_reconnect();
        }
    }

    // if (!doc["tipo"].isNull())
    // {
    //     const char *tipo = doc["tipo"];

    //     Serial.printf("#%s %s\r\n", parametros.id, tipo);
    //     // mqtt_print(("New type: " + newType).c_str());
    // }

    if (!doc["web"].isNull())
    {
        const String newWeb = doc["web"];
        if (newWeb == "on")
        {
            startWifiManagerWeb();
        }
        else if (newWeb == "off")
        {
            stopWifiManagerWeb();
        }
    }


    if (!doc["OTA"].isNull())
    {

        const String URL_OTA = doc["OTA"];

        t_httpUpdate_return ret;

        // mqtt_print(("[" + ID + "] Descargando firmware de " + server_host + ":" + port + server_uri).c_str());
        mqtt_print(("{\"ID\":\"" + String(parametros.id) + "\",\"OTA\":\"" + URL_OTA + "\"}").c_str());

        ret = ESPhttpUpdate.update(wifi_client, URL_OTA);

        mqtt_reconnect();

        switch (ret)
        {
        case HTTP_UPDATE_FAILED:
            PRINTF("[update] Update failed.\r");
            mqtt_print(("{\"ID\":\"" + String(parametros.id) + "\",\"OTA\":\"failed\"}").c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            PRINTF("[update] Update no Update.\r");
            break;
        case HTTP_UPDATE_OK:
            PRINTF("[update] Update ok.\r"); // may not called we reboot the ESP
            mqtt_print(("{\"ID\":\"" + String(parametros.id) + "\",\"OTA\":\"ok\"}").c_str());
            delay(1000);
            ESP.restart();
            break;
        }
    }
    if (!doc["OTAF"].isNull())
    {

        const String URL_OTA = doc["OTAF"];

        // mqtt_print(("[" + ID + "] Descargando firmware de " + server_host + ":" + port + server_uri).c_str());
        mqtt_print(("{\"ID\":\"" + String(parametros.id) + "\",\"OTAF\":\"" + URL_OTA + "\"}").c_str());

        t_httpUpdate_return ret;

        ret = ESPhttpUpdate.updateFS(wifi_client, URL_OTA);

        mqtt_reconnect();

        switch (ret)
        {
        case HTTP_UPDATE_FAILED:
            PRINTF("[update] Update failed.\r");
            mqtt_print(("{\"ID\":\"" + String(parametros.id) + "\",\"OTAF\":\"failed\"}").c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            PRINTF("[update] Update no Update.\r");
            break;
        case HTTP_UPDATE_OK:
            PRINTF("[update] Update ok.\r"); // may not called we reboot the ESP
            mqtt_print(("{\"ID\":\"" + String(parametros.id) + "\",\"OTAF\":\"ok\"}").c_str());
            delay(1000);
            ESP.restart();
            break;
        }
    }
}

void mqtt_print(const char *payload)
{
    PRINTF("Publicando en %s\r\n", topic_status);
    mqtt_client.publish(topic_status, payload);
}

void mqtt_disconnect()
{
    mqtt_client.disconnect();
}