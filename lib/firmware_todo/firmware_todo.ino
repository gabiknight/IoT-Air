#include <SPI.h>
#include <Wire.h>
//si se usa el nodemcu, hay que considerar cambiar en el MutichannelGasSensor.cpp y //sht21.cpp, el Wire.begin(); por Wire.begin(2,0);
#include "MutichannelGasSensor.h"
//#include "//sht21.h"
#include <SoftwareSerial.h>
//#include"Arduino.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUDP.h>
#include <EEPROM.h>
#include <string.h>



#define LED_ 16


#define WIFISSID "motopablo" // Put here your Wi-Fi SSID
#define PASSWORD "pabloibaceta" // Put here your Wi-Fi password


#define BUTTON_BUILTIN 0
#define LED_BUILTIN 2



const int read_particle_measurement[] = {0x68, 0x01, 0x04, 0x93};
const int start_particle_measurement[] = {0x68, 0x01, 0x01, 0x96};
const int stop_particle_measurement[] = {0x68, 0x01, 0x02, 0x95};
const int set_customer_adjustment[] = {0x68, 0x02, 0x08, '\0'};
const int read_customer_adjustment[] = {0x68, 0x01, 0x10, 0x87};
const int stop_auto_send[] = {0x68, 0x01, 0x20, 0x77};
const int enable_auto_send[] = {0x68, 0x01, 0x40, 0x57};

#define COMMAND_SIZE 4


int data_in[50] = {0};
int auxiliar;
char ID_EQUIPO[5] =  {'0', '5', '2', '\0', 0};
String VERSION_FIRMWARE = "001";
char string_auxiliar[10];
bool enviar = 0;
int datos = 0;
long air_pollution;
char trama[150];
char wifi_ssid[30];
char wifi_pass[30];
//RLINK lun 05 / 11:am
uint16_t port = 6791;
char * host = "168.194.207.66"; // ip or dns por defecto este ip (es para que no quede vacio, pero mas adelante se carga con el valor que haya almacenado "ip_del_servidor"

boolean error_en_el_servidor_ntp = 0;
//CEGA
//uint16_t port = 5000;
//char * host = "200.032.046.054"; // ip or dns por defecto este ip (es para que no quede vacio, pero mas adelante se carga con el valor que haya almacenado "ip_del_servidor"


////sht21 //sht21;


#define PM


void setup()
{
  pinMode(LED_, OUTPUT);
  Serial.begin(9600);
  // client.setDebug(true);

  EEPROM.begin(512);
  // client.setDebug(true);
  leer_config_actual();
  //Serial.println("Esperando pulso de boton FLASH para configuracion general.");

  for (int parp = 0; parp < 100; parp++) {
    digitalWrite(LED_, HIGH);
    if (digitalRead(BUTTON_BUILTIN) == 0) {
      recibir_configuracion_por_USB(); break;
    }
    delay(50);
    digitalWrite(LED_, LOW);
    if (digitalRead(BUTTON_BUILTIN) == 0) {
      recibir_configuracion_por_USB(); break;
    }
    delay(50);
  }


  WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  WiFi.begin(wifi_ssid, wifi_pass);
  while (Serial.available()) {
    char vaciobuffer = Serial.read();
    delay(20);
  }
  chequear_conexion();

  //pinMode(A0,INPUT);
  gas.begin(0x04);//the default I2C address of the slave is 0x04

  gas.powerOn();
  ////sht21.begin();


  delay(1000);


}


void loop()
{

  float NH3;
  float CO;
  float NO2;
  float C3H8;
  float C4H10;
  float CH4;
  float H2;
  float C2H5OH;
  //float //sht21_TEMP;
  //float //sht21_HUM;
  float DUST_SENSOR_pm25;
  float DUST_SENSOR_pm10;

  float c;
  float pm25;
  float pm10;
  int mediciones;

  int  indice = 0;


#ifdef PM
  //////////////////////////////////
  // Vacío el buffer de entrada
  while (Serial.available()) {

    data_in[indice] = Serial.read();

#ifdef DEBUG
    //debug_port.write(data_in[indice]);
#endif

    indice++;
  }
  indice = 0;

  ////////////////////////////////////////
  // Detengo el auto send del dust sensor

  for (auxiliar = 0; auxiliar < COMMAND_SIZE; auxiliar ++) Serial.write(stop_auto_send[auxiliar]);

  delay(500);

  while (Serial.available()) {
    data_in[indice] = Serial.read();

#ifdef DEBUG
    debug_port.write(data_in[indice]);
#endif

    indice++;
  }
  indice = 0;

  /////////////////////////////////////////
  // Lo vuelvo a mandar por las dudas...

  for (auxiliar = 0; auxiliar < COMMAND_SIZE; auxiliar ++) Serial.write(stop_auto_send[auxiliar]);

  delay(500);

  while (Serial.available()) {
    data_in[indice] = Serial.read();

#ifdef DEBUG
    debug_port.write(data_in[indice]);
#endif

    indice++;
  }
  indice = 0;

  /////////////////////////////////////////////////////////////////////////
  // Comiezo la medición de polvo, esto hace arrancar el cooler del sensor

  for (auxiliar = 0; auxiliar < COMMAND_SIZE; auxiliar ++) Serial.write(start_particle_measurement[auxiliar]);

  while (Serial.available()) {
    data_in[indice] = Serial.read();

#ifdef DEBUG
    debug_port.write(data_in[indice]);
#endif

    indice++;
  }
  indice = 0;

  delay(200);

#endif

  ////////////////////////////////////////////////////
  // Medicion de gases

  NH3 = gas.measure_NH3();
  // Serial.print("The concentration of NH3 is ");
  //Serial.println(NH3);
  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);


  CO = gas.measure_CO();
  // Serial.print("The concentration of CO is ");
  //Serial.println(CO);

  delay(200);    digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);

  NO2 = gas.measure_NO2();
  //Serial.print("The concentration of NO2 is ");


  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);

  C3H8 = gas.measure_C3H8();
  //Serial.print("The concentration of C3H8 is ");


  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);


  C4H10 = gas.measure_C4H10();
  //Serial.print("The concentration of C4H10 is ");


  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);

  CH4 = gas.measure_CH4();
  // Serial.print("The concentration of CH4 is ");
  // Serial.println(CH4);

  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);

  H2 = gas.measure_H2();
  //Serial.print("The concentration of H2 is ");


  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);

  C2H5OH = gas.measure_C2H5OH();
  //Serial.print("The concentration of C2H5OH is ");
  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);


  //////////////////////////////////////////////////
  // Medicion de humedad y temperatura

  //Serial.print("Humidity(%RH): ");
  //sht21_HUM = //sht21.getHumidity();
#ifdef DEBUG
  //debug_port.print(//sht21_HUM);
  //  Serial.println(//sht21_HUM);
#endif


  //Serial.print("     Temperature(C): ");
  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);
  //sht21_TEMP = //sht21.getTemperature();
  // Serial.println(//sht21_TEMP);
#ifdef DEBUG
  //debug_port.print(//sht21_TEMP);
#endif
  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(100);
  delay(7000); // espero para hacer la medicion

#ifdef PM

  /////////////////////////////////////////////////
  // Me fijo si quedo algo de basura en el buffer
  // y si hay algo lo vacío

  if (Serial.available()) {
    while (Serial.available()) {
      Serial.read();
    }
  }

  //////////////////////////////////////////////////////////////
  // Hago 25 mediciones y leo la última, sino tira una medición
  // que no es correcta y es siempre el mismo valor

  for (mediciones = 0; mediciones < 40; mediciones ++) {
    for (auxiliar = 0; auxiliar < COMMAND_SIZE; auxiliar++) {
      Serial.write(read_particle_measurement[auxiliar]);
    }
    //delay(500);
    digitalWrite(LED_, HIGH);
    delay(100);
    digitalWrite(LED_, LOW);
    delay(100);
    digitalWrite(LED_, HIGH);
    delay(100);
    digitalWrite(LED_, LOW);
    delay(100);
    indice = 0;

    while (Serial.available()) {
      data_in[indice] = Serial.read();

#ifdef DEBUG
      debug_port.write(data_in[indice]);
#endif
      indice++;
    }
  }

  int con;
  for (con = 0; con < indice; con ++) {
    if ((data_in[con] == 0x40) && (data_in[con + 1] == 0x05) && (data_in[con + 2] == 0x04)) {
      DUST_SENSOR_pm25 = (float) (data_in[con + 3] * 256 + data_in[con + 4]);
      DUST_SENSOR_pm10 = (float)(data_in[con + 5] * 256 + data_in[con + 6]);
    }
  }



#ifdef DEBUG
  debug_port.print(pm25);
#endif


#ifdef DEBUG
  debug_port.print("!!");
  debug_port.print(pm10);
#endif

  ////////////////////////////////////////////////////////////
  // Detengo la medición de partículas de polvo, esto
  // detiene el cooler del sensor

  indice = 0;

  for (auxiliar = 0; auxiliar < COMMAND_SIZE; auxiliar ++) Serial.write(stop_particle_measurement[auxiliar]);
  delay(500);
  while (Serial.available()) {
    data_in[indice] = Serial.read();

#ifdef DEBUG
    debug_port.write(data_in[indice]);
#endif
    indice++;
  }
  indice = 0;

  ///////////////////////////////////////////////////////////////
  // Vuelvo a detener la medición de partículas para asegurarme
  // de que se detenga


  for (auxiliar = 0; auxiliar < COMMAND_SIZE; auxiliar ++) Serial.write(stop_particle_measurement[auxiliar]);
  delay(500);
  while (Serial.available()) {
    data_in[indice] = Serial.read();

#ifdef DEBUG
    debug_port.write(data_in[indice]);
#endif
    indice++;
  }
  indice = 0;//

#endif


  char horaArray [15] ;
  String hora_string = obtener_string_hora_servidor_ntp();
  hora_string.toCharArray(horaArray, 15);


  digitalWrite(LED_, HIGH);
  delay(100);
  digitalWrite(LED_, LOW);
  delay(500);


  //char* buffloat = "";
  char buffloat[10];
  //ejemplo de como tiene que ser la trama:
  /*
    $,001,20180806010322,T=23.00,H=50.00,NH3=XX.XX,CO=XX.XX,NO2=XX.XX,C3H8=xx.xx,C4H10=XX.XX,CH4=XX.XX,H2=XX.XX,CH2H5OH=XX.XX,DUST=XX.XX,%
  */
  Serial.println(trama);
  strcpy(trama, "$");
  strcat(trama, ",01,");         // Tener cuidado con los strcat, el tamaño del string a copiar debe tener un numero mas que el que se quiere copiar
  strcat(trama, ID_EQUIPO);
  strcat(trama, ",");
  //strcat(trama, "20180000000000");
  strcat(trama, horaArray);
  strcat(trama, ",");
  //dtostrf(//sht21_TEMP, 4, 2, buffloat); //T=
  strcat(trama, buffloat);
  strcat(trama, ",");
  //dtostrf(//sht21_HUM, 4, 2, buffloat); //H=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(NH3, 4, 2, buffloat); //NH3=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(CO, 4, 2, buffloat); //CO=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(NO2, 4, 2, buffloat); //NO2=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(C3H8, 4, 2, buffloat); //C3H8=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(C4H10, 4, 2, buffloat); //C4H10=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(CH4, 4, 2, buffloat); //CH4=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(H2, 4, 2, buffloat); //H2=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(C2H5OH, 4, 2, buffloat); //C2H5OH=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(DUST_SENSOR_pm25, 4, 2, buffloat); //DUSTPM25=
  strcat(trama, buffloat);
  strcat(trama, ",");
  dtostrf(DUST_SENSOR_pm10, 4, 2, buffloat); //DUSTPM10=
  strcat(trama, buffloat);
  strcat(trama, ",");
  strcat(trama, "%");
  strcat(trama, "\0");
  if (error_en_el_servidor_ntp == 0) {
    enviar_datos_por_UDP();
    delay(10000);
    delay(10000);
  }
  delay(10000);
  //delay(10000);
  //delay(10000);
  enviar = 1;


}

void enviar_datos_por_UDP() {
  char incomingPacket[255];
  WiFiUDP UDP; //uso la clase WifiUDP para conectarme por udp.
  UDP.begin(6791);
  if (WiFi.status() == WL_CONNECTED) {
    ///////////////////////////aca viene el codigo para enviar al host///////////////////////
    // Serial.print("enviando por UDP a ");
    //  Serial.print(host); Serial.print(':'); Serial.println(port);
    // Serial.println(trama);
    UDP.beginPacket(host, port);
    UDP.write(trama);
    // UDP.endPacket();
    delay(100);
    ////////////////////////////////////

    //Serial.write("mi puerto es:");
    //Serial.println(UDP.localPort());
    int packetSize = UDP.parsePacket();
    if (packetSize)
    {
      // receive incoming UDP packets
      //Serial.print("Received ");
      //Serial.print(packetSize);
      //Serial.print("bytes from");
      //Serial.print(UDP.remoteIP().toString().c_str());
      //Serial.println(UDP.remotePort());

      int len = UDP.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = 0;
      }
      // Serial.print("UDP packet contents: ");
      // Serial.println(incomingPacket);
    }
    ///////////////////////////
    UDP.endPacket();
  }
}

void chequear_conexion() {
  int contconexion = 0;
  while (WiFi.status() != WL_CONNECTED) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(250);
    // Serial.print(".");
    digitalWrite(LED_, HIGH);
    delay(250);
    digitalWrite(LED_, LOW);
    if (contconexion > 50) {
      //     Serial.println("");
      //   Serial.println("Error de conexion al WIFI.. REINICIANDO MODULO");
      digitalWrite(LED_, LOW);
      //    Serial.println("");
      contconexion = 0;
      ESP.restart();
    }
  }
  contconexion = 0;
}

String obtener_string_hora_servidor_ntp() {
  int reintentos = 3;
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", -10800, 60000);//el -10800 es para que quede en GMT -3

  const uint8_t daysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  uint8_t yOff, m, d, hh, mm, ss;
  unsigned long t;
  error_en_el_servidor_ntp = 0;
  while (reintentos > 0) {
    timeClient.begin();
    timeClient.update();
    t = timeClient.getEpochTime();
    //Serial.print(t); Serial.print('-');
    if (t >= 1514764800 and t <= 4102444800) {
      break;
    }
    t = 1514764800;
    reintentos--;
    timeClient.end();
    delay(100);
  }
  if (reintentos == 0) {
    error_en_el_servidor_ntp = 1;
  }


  t -= 946684800;    // bring to 2000 timestamp from 1970
  ss = t % 60;
  t /= 60;
  mm = t % 60;
  t /= 60;
  hh = t % 24;
  uint16_t days = t / 24;
  uint8_t leap;
  for (yOff = 0; ; ++yOff) {
    leap = yOff % 4 == 0;
    if (days < 365 + leap)
      break;
    days -= 365 + leap;
  }
  for (m = 1; ; ++m) {
    uint8_t daysPerMonth = daysInMonth [m - 1];
    if (leap && m == 2)
      ++daysPerMonth;
    if (days < daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  d = days + 1;

  String yearStr = yOff < 10 ? "0" + String(yOff) : String(yOff);
  String monthStr = m < 10 ? "0" + String(m) : String(m);
  String dayStr = d < 10 ? "0" + String(d) : String(d);
  String hoursStr = hh < 10 ? "0" + String(hh) : String(hh);
  String minuteStr = mm < 10 ? "0" + String(mm) : String(mm);
  String secondStr = ss < 10 ? "0" + String(ss) : String(ss);

  return "20" + yearStr + monthStr + dayStr + hoursStr + minuteStr + secondStr;
  timeClient.end();
}

void recibir_configuracion_por_USB() {
  String wifi_ssid_string;
  String wifi_pass_string;
  Serial.println();
  Serial.print("--CONFIGURACION DEL DISPOSITIVO-- ID:");Serial.println(ID_EQUIPO);
  Serial.println("valores actuales:");
  Serial.print("SSID: "); Serial.println(wifi_ssid);
  Serial.print("PASS: "); Serial.println(wifi_pass);
  Serial.println("Ingrese la SSID del router WiFi");
  while (Serial.available() == 0) {}
  wifi_ssid_string = Serial.readString();
  Serial.println(wifi_ssid_string);
  Serial.flush();
  Serial.println("Ingrese la contrasena del router WiFi");
  while (Serial.available() == 0) {}
  wifi_pass_string = Serial.readString();
  Serial.println(wifi_pass_string);
  Serial.flush();

  grabar(0, wifi_ssid_string);
  grabar(50, wifi_pass_string);
  Serial.println("--CONFIGURACION TERMINADA--");
  Serial.println("valores configurados:");
  leer_config_actual();
  Serial.print("SSID: "); Serial.println(wifi_ssid);
  Serial.print("PASS: "); Serial.println(wifi_pass);
  Serial.println("Si los valores son incorrectos reinicie y vuelva a ingresar con el boton FLASH.");
  delay(1000);
  Serial.println("Iniciando Sistema..");
  Serial.println(); Serial.println(); Serial.println();
  Serial.print("SSID a la que se conecta:"); Serial.println(wifi_ssid);
  Serial.print("PASS a la que se conecta:"); Serial.println(wifi_pass);
  Serial.print("IP a la que se conecta:"); Serial.println(host);
  Serial.print("PUERTO al que se conecta:"); Serial.println(port);
}

void leer_config_actual() {
  leer(0).toCharArray(wifi_ssid, 50);
  leer(50).toCharArray(wifi_pass, 50);
  // leer(100).toCharArray(ip_del_servidor, 50);
}



//----------------Función para grabar en la EEPROM-------------------
void grabar(int addr, String a) {
  int tamano = a.length();
  char inchar[50];
  a.toCharArray(inchar, tamano + 1);
  for (int i = 0; i < tamano; i++) {
    EEPROM.write(addr + i, inchar[i]);
  }
  for (int i = tamano; i < 50; i++) {
    EEPROM.write(addr + i, 255);
  }
  EEPROM.commit();
}

//-----------------Función para leer la EEPROM------------------------
String leer(int addr) {
  byte lectura;
  String strlectura;
  for (int i = addr; i < addr + 50; i++) {
    lectura = EEPROM.read(i);
    if (lectura != 255) {
      strlectura += (char)lectura;
    }
  }
  return strlectura;
}
