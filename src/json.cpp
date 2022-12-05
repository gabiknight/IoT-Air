#include <header.h>

void crearJson()
{
    StaticJsonDocument<384> doc;

    char json_values[384];

    // 20220523203501
    doc["fecha"] = date.substring(6, 8) + '/' + date.substring(4, 6) + '/' + date.substring(0, 4) + " " + date.substring(8, 10) + ":" + date.substring(10, 12) + ":" + date.substring(12, 14);
    if (mAmbi.temperatura_am != 0)
        doc["t_a"] = String(mAmbi.temperatura_am, 2);
    if (mAmbi.temperatura_sht != 0)
        doc["t_s"] = String(mAmbi.temperatura_sht, 2);
    if (mAmbi.humedad_am != 0)
        doc["h_a"] = String(mAmbi.humedad_am, 2);
    if (mAmbi.humedad_sht != 0)
        doc["h_s"] = String(mAmbi.humedad_sht, 2);

    if (mAmbi.co2_ppm != 0)
        doc["co2_eq"] = mAmbi.co2_ppm;
#ifdef AMBIMATE
    doc["voc"] = mAmbi.voc_ppm;
#endif
    if (mAmbi.pm2_5 != 0)
        doc["pm25"] = mAmbi.pm2_5;
    if (mAmbi.pm2_5_plant != 0)
        doc["pm25_p"] = mAmbi.pm2_5_plant;
    if (mAmbi.pm10 != 0)
        doc["pm10"] = mAmbi.pm10;
    if (mAmbi.pm10_plant != 0)
        doc["pm10_p"] = mAmbi.pm10_plant;
    if (mAmbi.co_ppm != 0)
        doc["co"] = mAmbi.co_ppm;
    if (mAmbi.no2_ppm != 0)
        doc["n02"] = String(mAmbi.no2_ppm, 2);
    if (mAmbi.nh3_ppm != 0)
        doc["nh3"] = mAmbi.nh3_ppm;

    serializeJson(doc, json_values);

    mqtt_print(json_values);
}