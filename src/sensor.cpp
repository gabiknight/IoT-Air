#include <header.h>

ambi_t mAmbi;

#ifdef HONEYWELL
extern HPMA115S0 my_hpm;
#endif

#ifdef PLANTPHENOL
extern SerialPM pms;
#endif

#ifdef SHT_SENSOR
SHT2x sht;
#endif

/*
$= Cabecera de trama.
XX= ID del tipo de dispositivo. “04” para este sensor.
XXX= ID único de sensor en todo el sistema.
DATE= Fecha de envío de trama desde sensor, formato: AAAAMMDDHHMMSS.
T= Temperatura en C°.
H= humedad_am en %.
LX= Intensidad de Luz en LX.
DB= Sonido en DB.
CO2= Dióxido de Carbono.
VOC= Compuestos Orgánicos Volátiles.
DUSTPM25= Partículas Flotantes PM 2.5.
DUSTPM10= Partículas Flotantes PM 10.0.
%= Fin de trama.

$,XX,XXX,DATE,T,H,LX,DB,CO2,VOC,PM25,PM10,%
 */
/*
void sensor_main(){
  if(app_timeout(mApp.timeout_sensor_ambi, TIMEOUT_SENSOR_AMBI)){
    mApp.timeout_sensor_ambi= millis();
    gas_sensor_read();
  }
}
*/

void sensor_print()
{
    Serial.println("(SENSOR)");
    Serial.println("Date:   \t\t" + date);
    Serial.println("T_am:      \t\t" + (String)mAmbi.temperatura_am);
    Serial.println("T_sht:      \t\t" + (String)mAmbi.temperatura_sht);
    Serial.println("H_am:      \t\t" + (String)mAmbi.humedad_am);
    Serial.println("H_sht:      \t\t" + (String)mAmbi.humedad_sht);
    Serial.println("L:      \t\t" + (String)mAmbi.luminosidad);
    Serial.println("A:      \t\t" + (String)mAmbi.audio);
    Serial.println("C02:    \t\t" + (String)mAmbi.co2_ppm);
    Serial.println("VOC:    \t\t" + (String)mAmbi.voc_ppm);
    Serial.println("PIR:    \t\t" + (String)mAmbi.pir);
    Serial.println("PM2.5:    \t\t" + (String)mAmbi.pm2_5);
    Serial.println("PM10:    \t\t" + (String)mAmbi.pm10);
    Serial.println("PM2.5_plant:    \t\t" + (String)mAmbi.pm2_5_plant);
    Serial.println("PM10_plant:    \t\t" + (String)mAmbi.pm10_plant);
    Serial.println("CO:    \t\t" + (String)mAmbi.co_ppm);
    Serial.println("NO2:    \t\t" + (String)mAmbi.no2_ppm);
    Serial.println("NH3:    \t\t" + (String)mAmbi.nh3_ppm);
}

void gas_sensor_init()
{
#ifdef AMBIMATE
    Wire.begin(D4, D3);
    // Data and basic information are acquired from the module
    Wire.beginTransmission(0x2A);       // transmit to device
    Wire.write(byte(0x80));             // sends instruction to read firmware version
    Wire.endTransmission();             // stop transmitting
    Wire.requestFrom(0x2A, 1);          // request 1 byte from slave device
    unsigned char fw_ver = Wire.read(); // receive a byte

    Wire.beginTransmission(0x2A);           // transmit to device
    Wire.write(byte(0x81));                 // sends instruction to read firmware subversion
    Wire.endTransmission();                 // stop transmitting
    Wire.requestFrom(0x2A, 1);              // request 1 byte from slave device
    unsigned char fw_sub_ver = Wire.read(); // receive a byte

    Wire.beginTransmission(0x2A);            // transmit to device
    Wire.write(byte(0x82));                  // sends instruction to read optional sensors byte
    Wire.endTransmission();                  // stop transmitting
    Wire.requestFrom(0x2A, 1);               // request 1 byte from slave device
    unsigned char opt_sensors = Wire.read(); // receive a byte

    mAmbi.estado = 0;
    mAmbi.temperatura_am = 0;
    mAmbi.temperatura_sht = 0;
    mAmbi.humedad_am = 0;
    mAmbi.luminosidad = 0;
    mAmbi.audio = 0;
    mAmbi.co2_ppm = 0;
    mAmbi.voc_ppm = 0;
    mAmbi.pir = 0;
    date = "";

    Serial.printf("Sensor iniciado. Version firmware %i.%i. Opcional %i.\n", fw_ver, fw_sub_ver, opt_sensors);
#endif

#ifdef MULTICHANNEL
    gas.begin(0x04);
    gas.powerOn();
#endif
}

void gas_sensor_read()
{
#ifdef AMBIMATE
    unsigned char dato[30];
    unsigned int i = 0;

    memset(dato, '\0', sizeof dato);
    Wire.beginTransmission(byte(0x2A)); // transmit to device
    Wire.write(byte(0xC0));             // sends instruction to read sensors in next byte
    Wire.write(byte(0x3F));             // Lectura de los sensores
    Wire.endTransmission();             // stop transmitting
    delay(100);
    Wire.beginTransmission(byte(0x2A));        // transmit to device
    Wire.write(byte(0x00));                    // sends instruction to read sensors in next byte
    Wire.endTransmission();                    // stop transmitting
    Wire.requestFrom(byte(0x2A), (uint8_t)15); // request bytes from slave device

    while (Wire.available())
    {                          // slave may send less than requested
        dato[i] = Wire.read(); // receive a byte as character
        i++;
    }
    delay(100);

    mAmbi.estado = dato[0];
    mAmbi.temperatura_am = ((float)(dato[1] * 256.0 + dato[2]) / (float)10.0);
    mAmbi.humedad_am = (float)(dato[3] * 256.0 + dato[4]) / (float)10.0;
    mAmbi.luminosidad = (dato[5] * 256.0 + dato[6]);
    mAmbi.audio = (dato[7] * 256.0 + dato[8]);
    mAmbi.co2_ppm = (dato[11] * 256.0 + dato[12]);
    mAmbi.voc_ppm = (dato[13] * 256.0 + dato[14]);
    if (mAmbi.estado & 0x80)
        mAmbi.pir = 1;

    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    if ((mAmbi.temperatura_am > 90) || (mAmbi.co2_ppm > 65000) || (mAmbi.co2_ppm < 400))
    {
        if (DEBUG_SENSOR)
        {
            Serial.println("(SENSOR) Error lectura");
            sensor_print();
        }
        return;
    }

#endif

    date = get_date_ntp();

#ifdef MULTICHANNEL
    mAmbi.co_ppm = (unsigned int)gas.measure_CO();
    mAmbi.no2_ppm = gas.measure_NO2();
    mAmbi.nh3_ppm = gas.measure_NH3();
#endif
}

void pm_sensor_init()
{
#ifdef HONEYWELL
    // digitalWrite(ENABLE, LOW);
    // delay(2000);
    // digitalWrite(ENABLE, HIGH);
    my_hpm.Init();
    delay(100);
    my_hpm.StartParticleMeasurement();
#endif

#ifdef PLANTPHENOL
    pms.init();
#endif
}

void stop_measuring_pm()
{
#ifdef HONEYWELL
    my_hpm.StopParticleMeasurement();
#endif
}

void pm_sensor_read()
{
#ifdef HONEYWELL
    if (my_hpm.ReadParticleMeasurement(&mAmbi.pm2_5, &mAmbi.pm10))
    {
        PRINTF("PM recibido correctamente.\n");
    }
    else
    {
        PRINTF("Measurement fail\n");
    }
#endif

#ifdef PLANTPHENOL
    pms.read();
    if (pms)
    {

        mAmbi.pm2_5_plant = pms.pm25;
        mAmbi.pm10_plant = pms.pm10;
        // print formatted results
        // Serial.printf("PM1.0 %2d, PM2.5 %2d, PM10 %2d [ug/m3]\n",
        //               pms.pm01, pms.pm25, pms.pm10);

        // if (pms.has_number_concentration())
        //     Serial.printf("N0.3 %4d, N0.5 %3d, N1.0 %2d, N2.5 %2d, N5.0 %2d, N10 %2d [#/100cc]\n",
        //                   pms.n0p3, pms.n0p5, pms.n1p0, pms.n2p5, pms.n5p0, pms.n10p0);

        // if (pms.has_temperature_humidity() || pms.has_formaldehyde())
        //     Serial.printf("%5.1f °C, %5.1f %%rh, %5.2f mg/m3 HCHO\n",
        //                   pms.temp, pms.rhum, pms.hcho);
    }
    else
    { // something went wrong
        switch (pms.status)
        {
        case pms.OK: // should never come here
            break;   // included to compile without warnings
        case pms.ERROR_TIMEOUT:
            PRINTF("%s\n", PMS_ERROR_TIMEOUT);
            break;
        case pms.ERROR_MSG_UNKNOWN:
            PRINTF("%s\n", PMS_ERROR_MSG_UNKNOWN);
            break;
        case pms.ERROR_MSG_HEADER:
            PRINTF("%s\n", PMS_ERROR_MSG_HEADER);
            break;
        case pms.ERROR_MSG_BODY:
            PRINTF("%s\n", PMS_ERROR_MSG_BODY);
            break;
        case pms.ERROR_MSG_START:
            PRINTF("%s\n", PMS_ERROR_MSG_START);
            break;
        case pms.ERROR_MSG_LENGTH:
            PRINTF("%s\n", PMS_ERROR_MSG_LENGTH);
            break;
        case pms.ERROR_MSG_CKSUM:
            PRINTF("%s\n", PMS_ERROR_MSG_CKSUM);
            break;
        case pms.ERROR_PMS_TYPE:
            PRINTF("%s\n", PMS_ERROR_PMS_TYPE);
            break;
        }
    }
#endif
}

void ht_sensor_init()
{
#ifdef SHT_SENSOR
    sht.begin(D4, D3);
#endif
}

void ht_sensor_read()
{
#ifdef SHT_SENSOR
    sht.read();

    mAmbi.humedad_sht = sht.getHumidity();
    mAmbi.temperatura_sht = sht.getTemperature();
#endif
}