
#include <header.h>
//!EN LIBRERIA DE MULTICHANNEL CAMBIAR POR Wire.begin(D4, D3);

WiFiClient espClient;

String payload;
String ID = "00", urlReport = "rlink.guaymallen.gob.ar/api/panels?";

// Variables to save values from HTML form
String ssid;
String pass;
String date;

// File paths to save input values permanently
const char *ssidPath = "/ssid.txt";
const char *passPath = "/pass.txt";
const char *idPath = "/id.txt";
const char *urlReportPath = "/urlReport.txt";

IPAddress localIP;
// IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
// IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000; // interval to wait for Wi-Fi connection (milliseconds)

bool restart = false;

int contador = 0;

SoftwareSerial Serial_hpm, Serial_apm;

#ifdef HONEYWELL
HPMA115S0 my_hpm(Serial_hpm);
#endif

#ifdef PLANTPHENOL
SerialPM pms(PMS7003, Serial_apm);
#endif

void setup()
{
  pinMode(D8, OUTPUT);
  
  // digitalWrite(D8, HIGH);
  //  Serial port for debugging purposes
  Serial.begin(9600);
  Serial_hpm.begin(9600, SWSERIAL_8N1, D7, D6, false); // Rx Tx
  Serial_apm.begin(9600, SWSERIAL_8N1, D5, D8, false);

  if (!Serial_hpm)
  { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid SoftwareSerial pin configuration, check config");
    while (1)
    { // Don't continue with invalid configuration
      delay(1000);
    }
  }

  ESPhttpUpdate.rebootOnUpdate(false);

  initFS(LittleFS);

  // Load values saved in LittleFS
  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);
  ID = readFile(LittleFS, idPath);
  urlReport = readFile(LittleFS, urlReportPath);
  // ip = readFile(LittleFS, ipPath);
  // gateway = readFile(LittleFS, gatewayPath);
  PRINTF("SSID: %s\n", ssid.c_str());
  PRINTF("PASS: %s\n", pass.c_str());

  if (initWiFi())
  {
    // startWifiManagerWeb();
    //  connecting to a mqtt broker
    set_MQTT_parameters();
    mqtt_reconnect();
    mqtt_print(("Hola soy " + ID + ". Ver " + VERSION).c_str());
  }
  else
  {
    startWifiManagerWeb();
  }

  gas_sensor_init();

  while (Serial_hpm.available() > 0)
  {
    Serial_hpm.read();
  }

  pm_sensor_init();

  ht_sensor_init();
}

void loop()
{
  if (restart)
  {
    delay(5000);
    ESP.restart();
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    mqtt_reconnect();

    mqtt_loop();
  }

  if ((millis() - previousMillis) > MEASURING_PERIOD_MS)
  {
    contador++;
    if (contador == 3)
    {
      //digitalWrite(D8, LOW);
      //delay(2000);
      contador = 0;
    }

    previousMillis = millis();

    gas_sensor_read();

    pm_sensor_read();

    ht_sensor_read();

    if (DEBUG_SENSOR)
    {
      sensor_print();
    }

    crearJson();
  }
}