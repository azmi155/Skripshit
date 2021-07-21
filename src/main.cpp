#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DFRobot_ESP_PH_WITH_ADC.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>

#ifdef __cplusplus
extern "C"
{
#endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

#define inPompa "1710510160@stmikbumigora.ac.id/controll/pompa"
#define inNutrisi "1710510160@stmikbumigora.ac.id/controll/nutrisi"
#define outMqtt "1710510160UlulAzmi/esp32/pompa"
#define pompa 17
#define nutrisi 16
#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 35       //the esp gpio data pin number
#define offset 1.5
#define DHTPIN 5
#define DHTTYPE DHT11
#define MSG_BUFFER_SIZE (50)
#define BUILTIN_LED 2
#define trigPin 27
#define echhoPin 26

//-------------------------------
WiFiUDP ntpUDP;
WiFiClient espClient;
PubSubClient client(espClient);
DFRobot_ESP_PH_WITH_ADC ph;
DHT_Unified dht(DHTPIN, DHTTYPE);
NTPClient timeClient(ntpUDP, "id.pool.ntp.org", 28800);
sensors_event_t event;

//----------WIFI----------------
const char *ssid = "azmi2";
const char *password = "12345678901";
const char *mqtt_server = "13.70.33.117";
const char *username = "nxewbvqt";
const char *pass = "wzWrC0HdsHhG";
float voltage, phValue, temperature = 25;
int stPompa;
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int vlaue = 0;
int value = 0;
char res[8];

long duration, suhu;
int distance;
uint32_t delayMS;
//NTPClient timeClient(ntpUDP);

void setup_wifi()
{

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(" . ");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("Wifi Connected");
  Serial.println("IP Address: ");
  Serial.print(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{

  Serial.print("Message Arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  if (String(topic) == inPompa)
  {
    Serial.print("Changing output to ");
    if (messageTemp == "pompa_on")
    {
      Serial.println("pompa_on");
      digitalWrite(BUILTIN_LED, HIGH);
      digitalWrite(pompa, LOW);
    }
    else if (messageTemp == "pompa_off")
    {
      Serial.println("pompa_off");
      digitalWrite(BUILTIN_LED, LOW);
      digitalWrite(pompa, HIGH);
    }
  }
  else if (String(topic) == inNutrisi)
  {
    Serial.print("Changing output to ");
    if (messageTemp == "nutrisi_on")
    {
      Serial.println("nutrisi_on");
      digitalWrite(BUILTIN_LED, HIGH);
      digitalWrite(nutrisi, LOW);
    }
    else if (messageTemp == "nutrisi_off")
    {
      Serial.println("nutrisi_off");
      digitalWrite(BUILTIN_LED, LOW);
      digitalWrite(nutrisi, HIGH);
    }
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-1710510160";

    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outMqtt, "Connected");
      // ... and resubscribe
      client.subscribe(inPompa);
      client.subscribe(inNutrisi);
      client.subscribe("ph");
      client.subscribe("1710510160@stmikbumigora.ac.id/sensor/suhu");
      client.subscribe("1710510160@stmikbumigora.ac.id/sensor/levelair");
      client.subscribe("1710510160@stmikbumigora.ac.id/sensor/suhuair");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echhoPin, INPUT);
  pinMode(pompa, OUTPUT);
  pinMode(nutrisi, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(DHTPIN, INPUT);
  Serial.begin(9600);
  digitalWrite(pompa, LOW);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ph.begin();
  dht.begin();
  timeClient.begin();
}

void loop()
{

  timeClient.update();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // unsigned long now = millis();
  // if (now - lastMsg > 2000)
  // {
  //   lastMsg = now;
  //   ++value;
  //   snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
  //   Serial.print("Publish message: ");
  //   Serial.println(msg);
  //   client.publish("1710510160UlulAzmi/esp32/v1", msg);
  //}

  //--------------------------------------------------

  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) //time interval: 1s
  {
    timepoint = millis();
    //voltage = rawPinValue / esp32ADC * esp32Vin
    Serial.println("-----------------");
    Serial.print("timestamp : ");
    Serial.println(timeClient.getFormattedTime());

    voltage = analogRead(PH_PIN) * 5.0 / 10240; // read the voltage
    Serial.print("voltage:");
    Serial.println(voltage, 4);
    dtostrf(voltage, 6, 3, res);
    // Serial.print("Publish message: ");
    snprintf(msg, MSG_BUFFER_SIZE, res, value);
    client.publish("1710510160UlulAzmi/esp32/voltase", msg);

    //temperature = readTemperature();  // read your temperature sensor to execute temperature compensation
    Serial.print("temperature:");
    Serial.print(temperature, 1);
    Serial.println("^C");
    dtostrf(temperature, 6, 3, res);
    // Serial.print("Publish message: ");
    snprintf(msg, MSG_BUFFER_SIZE, res, value);
    client.publish("1710510160@stmikbumigora.ac.id/sensor/suhuair", msg);

    //phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
    phValue = 3.5 * voltage + offset;
    Serial.print("pH:");
    Serial.println(phValue, 4);
    dtostrf(phValue, 6, 3, res);
    // Serial.print("Publish message: ");
    snprintf(msg, MSG_BUFFER_SIZE, res, value);
    client.publish("ph", msg);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echhoPin, HIGH);
    distance = duration * 0.0343 / 2;
    Serial.print(distance);
    Serial.println(" cm");
    dtostrf(distance, 3, 0, res);
    // Serial.print("Publish message: ");
    snprintf(msg, MSG_BUFFER_SIZE, res, value);
    client.publish("1710510160@stmikbumigora.ac.id/sensor/levelair", msg);

    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
      Serial.println(F("Error Membaca Temperatur!"));
    }
    else
    {
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));
      dtostrf(event.temperature, 3, 0, res);
      snprintf(msg, MSG_BUFFER_SIZE, res, value);
      client.publish("1710510160@stmikbumigora.ac.id/sensor/suhu", msg);
    }
    //  Membaca Kelembababn
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
      Serial.println(F("Error Membaca Kelembaban!"));
    }
    else
    {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
      dtostrf(event.relative_humidity, 3, 0, res);
      snprintf(msg, MSG_BUFFER_SIZE, res, value);
      client.publish("1710510160@stmikbumigora.ac.id/sensor/kelembaban", msg);
    }

    //--------------------------------------------------------------------------

    //------------------------------------------------------------------------------
  }
  ph.calibration(voltage, temperature);
}

//sini

// float uAnggota, uRendah, uSedang, uTinggi,
//     uAsam, uNetral, uBasa, uDingin, uNormal, uPanas;

// float keluaranSuhu;
// float coa_ph;
// float keluaranPh;

// float kondisiSuhu;
// float kondisiPh;
// float kondisiVolume;

// void hitung_anggota(int anggota, float Nilai, float A, float B, float C)
// {
//   switch (anggota)
//   {
//   case 1:
//     if ((Nilai >= A) && (Nilai <= B))
//       uAnggota = 1;
//     if ((Nilai > B) && (Nilai < C))
//       uAnggota = (Nilai - A) / (B - A);
//     if ((Nilai >= A) && (Nilai <= B))
//       uAnggota = 0;
//     break;
//   case 2:
//     if ((Nilai >= A) && (Nilai <= B))
//       uAnggota = 1;
//     if ((Nilai > B) && (Nilai < C))
//       uAnggota = (Nilai - A) / (B - A);
//     if ((Nilai >= A) && (Nilai <= B))
//       uAnggota = 0;
//     break;
//   case 3:
//     if ((Nilai >= A) && (Nilai <= B))
//       uAnggota = 1;
//     if ((Nilai > B) && (Nilai < C))
//       uAnggota = (Nilai - A) / (B - A);
//     if ((Nilai >= A) && (Nilai <= B))
//       uAnggota = 0;
//     break;
//   }
// }

// //fuzzyfikasi
// void fuzzy()
// {
//   // int uAnggota;
//   //tinggi air
//   //angka 4 diganti pake variabel yg nampung air
//   uAnggota = 0;
//   hitung_anggota(1, distance, 0, 3, 6);
//   uRendah = uAnggota;
//   hitung_anggota(2, distance, 3, 6, 9);
//   uSedang = uAnggota;
//   hitung_anggota(3, distance, 6, 9, 9);
//   uTinggi = uAnggota;

//   //Ph
//   uAnggota = 0;
//   hitung_anggota(1, phValue, 0, 3, 6);
//   uAsam = uAnggota;
//   hitung_anggota(2, phValue, 3, 6, 9);
//   uNetral = uAnggota;
//   hitung_anggota(3, phValue, 6, 9, 9);
//   uBasa = uAnggota;

//   //suhu
//   uAnggota = 0;
//   hitung_anggota(1, event.temperature, 0, 3, 6);
//   uDingin = uAnggota;
//   hitung_anggota(2, event.temperature, 3, 6, 9);
//   uNormal = uAnggota;
//   hitung_anggota(3, event.temperature, 6, 9, 9);
//   uPanas = uAnggota;
// }

// float fuzzy_setSuhu[3][3] = {
//     {15, 15, 35},
//     {15, 35, 50},
//     {35, 50, 50},
// };

// float fuzzy_setPh[3][3] = {
//     {15, 15, 35},
//     {15, 35, 50},
//     {35, 50, 50},
// };

// void defuzzyfikasi()
// {
//   float pembilangSuhu = 0, penyebutSuhu = 0,
//         pembilangPh = 0, penyebutPh = 0, coa_suhu = 0, coa_ph;

//   float N_suhu[3] = {};
//   float N_ph[3] = {};
//   float N_volume[3] = {};

//   for (int set = 0; set < 9;)
//   {
//     for (int i = 0; i < 3; i++)
//     {
//       for (int j = 0; j < 3; j++)
//       {
//         //suhu
//         float data_uSuhu[3] = {uDingin, uNormal, uPanas};
//         N_suhu[i] = {data_uSuhu[i]};

//         //ph
//         float data_uPh[3] = {uAsam, uNetral, uBasa};
//         N_ph[i] = {data_uPh[i]};

//         //volume
//         float data_uVolume[3] = {uRendah, uSedang, uTinggi};
//         float N_volume[i] = {data_uVolume[i]};

//         kondisiSuhu = max(N_suhu[i], kondisiSuhu);
//         kondisiPh = max(N_ph[i], kondisiPh);
//         kondisiVolume = max(N_volume[i], kondisiVolume);

//         //coa
//         //suhu
//         float Min_suhu[set] = {min(N_suhu[i], N_volume[j])};
//         float pembilangSuhu =+ Min_suhu[set] * fuzzy_setSuhu[i][j];
//         float penyebutSuhu =+ Min_suhu[set];

//         //ph
//         float Min_ph[set] = {min(N_ph[i], N_volume[j])};
//         pembilangPh =+Min_ph[set] * fuzzy_setPh[i][j];
//         penyebutPh =+Min_ph[set];
//         delay(5);
//         set++;
//       }
//     }
//     //coa
//     coa_suhu = pembilangSuhu / penyebutSuhu;
//     keluaranSuhu = coa_suhu;

//     coa_ph = pembilangPh / penyebutPh;
//     keluaranPh = coa_ph;
//   }
// }
// String OutputFuzzySuhu;

// void basis_aturan_fuzzySuhu()
// {
//   if (kondisiSuhu == uDingin && kondisiVolume == uRendah)
//   {
//     OutputFuzzySuhu = "Lambat";
//     Serial.println(OutputFuzzySuhu);
//   }
//   else if (kondisiSuhu == uDingin && kondisiVolume == uSedang)
//   {
//     OutputFuzzySuhu = "Lambat";
//     Serial.println(OutputFuzzySuhu);
//   }
//   else if (kondisiSuhu == uDingin && kondisiVolume == uTinggi)
//   {
//     OutputFuzzySuhu = "Sedang";
//     Serial.println(OutputFuzzySuhu);
//   }
// }

// void basis_aturan_fuzzyPh()
// {
//   if (kondisiSuhu == uDingin && kondisiVolume == uRendah)
//   {
//     OutputFuzzySuhu = "Lambat";
//     Serial.println(OutputFuzzySuhu);
//   }
//   else if (kondisiSuhu == uDingin && kondisiVolume == uSedang)
//   {
//     OutputFuzzySuhu = "Lambat";
//     Serial.println(OutputFuzzySuhu);
//   }
//   else if (kondisiSuhu == uDingin && kondisiVolume == uTinggi)
//   {
//     OutputFuzzySuhu = "Sedang";
//     Serial.println(OutputFuzzySuhu);
//   }
// }
