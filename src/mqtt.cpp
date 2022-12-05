#include <header.h>

// MQTT Broker
const char *mqtt_broker = "168.194.207.67";
const char *topic = "gc/sensor3";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;
extern WiFiClient espClient;
PubSubClient client(espClient);
extern String ID, urlReport;
int delayms = 5000; // Tiempo entre consultas del estado

void mqtt_loop()
{
    client.loop();
}

void set_MQTT_parameters()
{
    client.setKeepAlive(1.5 * delayms);     // KeepAlive de conexion MQTT en segundos
    client.setSocketTimeout(1.5 * delayms); // TimeOut de conexion MQTT en segundos
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
}

void mqtt_reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        PRINTF("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("sensor_3")) // ID del cliente
        {
            PRINTF("connected\n");

            client.subscribe(topic);
        }
        else
        {
            PRINTF("failed, rc= %i try again in 5 seconds", client.state());
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void configRegistration()
{ // Suscripción al topic del puesto
    // envieMensajeConfig = false;               //Si no nos hemos suscripto entonces suponemos que no nos hemos registrado
    String topicPuesto = "SAC/"; // Arma el topic al que se va a suscribir (bikelocker/"MAC")
    PRINTF("Me voy a suscribir a: %s", topicPuesto.c_str());

    client.subscribe(topicPuesto.c_str()); // Se suscribe al topic
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
        const String newID = doc["id"];
        if (ID != newID)
        {
            Serial.printf("#%s a #%s\r\n", ID.c_str(), newID.c_str());
            ID = newID;
            writeFile(LittleFS, idPath, ID.c_str());
            WiFi.softAP("SAC " + ID);
            mqtt_print(("New ID: " + ID).c_str());
        }
    }
    

    if (!doc["URLReport"].isNull())
    {
        const String newURLReport = doc["URLReport"];
        if (urlReport != newURLReport)
        {
            urlReport = newURLReport;
            Serial.printf("#%s URLREP %s\r\n", ID.c_str(), urlReport.c_str());
            writeFile(LittleFS, urlReportPath, urlReport.c_str());
            mqtt_print("Nueva URLReport recibida");
        }
    }


    if (!doc["OTA"].isNull())
    {

        const String URL_OTA = doc["OTA"];

        client.unsubscribe(topic);

        int indx1 = URL_OTA.indexOf("://");
        int indx2 = URL_OTA.indexOf(":", indx1 + 1);
        t_httpUpdate_return ret;

        const String server_host = URL_OTA.substring(indx1 + 3, indx2);

        int indx3 = URL_OTA.indexOf("/", indx2);
        int port = URL_OTA.substring(indx2 + 1, indx3).toInt();

        const String server_uri = URL_OTA.substring(indx3);

        client.publish(topic, ("Descargando firmware de " + server_host + ":" + port + server_uri).c_str());

        ret = ESPhttpUpdate.update(espClient, server_host, port, server_uri);

        switch (ret)
        {
        case HTTP_UPDATE_FAILED:
            PRINTF("[update] Update failed.");
            break;
        case HTTP_UPDATE_NO_UPDATES:
            PRINTF("[update] Update no Update.");
            break;
        case HTTP_UPDATE_OK:
            PRINTF("[update] Update ok."); // may not called we reboot the ESP
            ESP.restart();
            break;
        }
    }
}

void mqtt_print(const char *payload)
{
    client.unsubscribe(topic);
    client.publish(topic, payload);
    client.subscribe(topic);
}