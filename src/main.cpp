#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DFRobot_ESP_PH_WITH_ADC.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#ifdef __cplusplus
extern "C"
{
#endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

#define inPompa "1710510160UlulAzmi/esp32/controll/pompa"
#define outMqtt "1710510160UlulAzmi/esp32/pompa"
#define pompa 17
#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 35       //the esp gpio data pin number
#define offset 1.5
float voltage, phValue, temperature = 25;
DFRobot_ESP_PH_WITH_ADC ph;

//----------WIFI----------------
const char *ssid = "azmi2";
const char *password = "12345678901";
const char *mqtt_server = "test.mosquitto.org";

int stPompa;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int vlaue = 0;
#define BUILTIN_LED 2
int value = 0;
char res[8];

#define trigPin 27
#define echhoPin 26

long duration, suhu;
int distance;

WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);
NTPClient timeClient(ntpUDP, "id.pool.ntp.org", 28800);

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
    if (messageTemp == "1")
    {
      Serial.println("on");
      digitalWrite(BUILTIN_LED, HIGH);
      digitalWrite(pompa, HIGH);
    }
    else if (messageTemp == "0")
    {
      Serial.println("off");
      digitalWrite(BUILTIN_LED, LOW);
      digitalWrite(pompa, LOW);
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
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outMqtt, "Connected");
      // ... and resubscribe
      client.subscribe(inPompa);
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
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  digitalWrite(pompa, LOW);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ph.begin();
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
    //voltage = analogRead(PH_PIN);
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
    client.publish("1710510160UlulAzmi/esp32/temperature", msg);

    //phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
    phValue = 3.5 * voltage + offset;
    Serial.print("pH:");
    Serial.println(phValue, 4);

    dtostrf(phValue, 6, 3, res);
    // Serial.print("Publish message: ");
    snprintf(msg, MSG_BUFFER_SIZE, res, value);
    client.publish("1710510160UlulAzmi/esp32/ph", msg);
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
    client.publish("1710510160UlulAzmi/esp32/whaterlevel", msg);

    suhu = (temprature_sens_read() - 32) / 1.8;

    Serial.print("Temperature: ");
    Serial.print(suhu);
    // Convert raw temperature in F to Celsius degrees
    // Serial.print((temprature_sens_read() - 32) / 1.8);
    Serial.println(" C");
    dtostrf(suhu, 3, 0, res);
    // Serial.print("Publish message: ");
    snprintf(msg, MSG_BUFFER_SIZE, res, value);
    client.publish("1710510160UlulAzmi/esp32/suhu", msg);
    delay(100);
  }
  ph.calibration(voltage, temperature);
}

float uAnggota, uRendah, uSedang, uTinggi,
uAsam, uNetral, uBasa, uDingin, uNormal, uPanas;

//fuzzyfikasi
void fuzzy()
{
  // int uAnggota;
  //tinggi air
  //angka 4 diganti pake variabel yg nampung air
  uAnggota = 0;
  hitung_anggota(1, 4, 0, 3, 6);
  uRendah = uAnggota;
  hitung_anggota(2, 4, 3, 6, 9);
  uSedang = uAnggota;
  hitung_anggota(3, 4, 6, 9, 9);
  uTinggi = uAnggota;

  uAnggota = 0;
  hitung_anggota(1, phValue, 0, 3, 6);
  uAsam = uAnggota;
  hitung_anggota(2, phValue, 3, 6, 9);
  uNetral = uAnggota;
  hitung_anggota(3, phValue, 6, 9, 9);
  uBasa = uAnggota;

  //suhu
  uAnggota = 0;
  hitung_anggota(1, suhu, 0, 3, 6);
  uDingin = uAnggota;
  hitung_anggota(2, suhu, 3, 6, 9);
  uNormal = uAnggota;
  hitung_anggota(3, suhu, 6, 9, 9);
  uPanas = uAnggota;
}

void hitung_anggota(int anggota, float Nilai, float A, float B, float C)
{
  switch (anggota)
  {
  case 1:
    if ((Nilai >= A) && (Nilai <= B))
      uAnggota = 1;
    if ((Nilai > B) && (Nilai < C))
      uAnggota = (Nilai - A) / (B - A);
    if ((Nilai >= A) && (Nilai <= B))
      uAnggota = 0;
    break;
  case 2:
    if ((Nilai >= A) && (Nilai <= B))
      uAnggota = 1;
    if ((Nilai > B) && (Nilai < C))
      uAnggota = (Nilai - A) / (B - A);
    if ((Nilai >= A) && (Nilai <= B))
      uAnggota = 0;
    break;
  case 3:
    if ((Nilai >= A) && (Nilai <= B))
      uAnggota = 1;
    if ((Nilai > B) && (Nilai < C))
      uAnggota = (Nilai - A) / (B - A);
    if ((Nilai >= A) && (Nilai <= B))
      uAnggota = 0;
    break;
  }
}

float fuzzy_setSuhu[3][3] = {
    {15, 15, 35},
    {15, 35, 50},
    {35, 50, 50},
};

float fuzzy_setPh[3][3] = {
    {15, 15, 35},
    {15, 35, 50},
    {35, 50, 50},
};

void
defuzzyfikasi(){
  float pembilangSuhu = 0, penyebutSuhu = 0,
        pembilangPh = 0, penyebutPh = 0, coa_suhu = 0, coa_ph;

  float N_suhu[3] = {};
  float N_ph[3] = {};
  float N_volume[3] = {};

  for (int set = 0; set < 9;)
  {
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        //suhu
        float data_uSuhu[3] = {uDingin, uNormal, uPanas};
        N_suhu[i] = data_uSuhu[i];

        //ph
        float data_uPh[3] = {uAsam, uNetral, uBasa};
        N_ph[i] = data_uPh[i];

        //volume
        float data_uVolume[3] = {uRendah, uSedang, uTinggi};
        float N_volume[i] = data_uVolume[i];

        float kondisiSuhu = max(N_suhu[i], kondisiSuhu);
        float kondisiPh = max(N_ph[i], kondisiPh);
        float kondisiVolume = max(N_volume[i], kondisiVolume);

        //coa
        //suhu
        float Min_suhu[set] = min(N_suhu[i], N_volume[j]);
        float pembilangSuhu =+ Min_suhu[set] * fuzzy_setSuhu[i][j];
        float penyebutSuhu =+ Min_suhu[set];

        //ph
        float Min_ph[set] = min(N_ph[i], N_volume[j]);
        float pembilangPh = +Min_ph[set] * fuzzy_setPh[i][j];
        float penyebutPh = +Min_ph[set];
        delay(5);
        set++;
      }
    }
    //coa

    coa_suhu = pembilangSuhu / penyebutSuhu;
    float keluaranSuhu = coa_suhu;

    float coa_ph = pembilangPh / penyebutPh;
    float keluaranPh = coa_ph;
  }
}

void basis_aturan_fuzzySuhu()
{
  if (kondisiSuhu == uDingin && kondisiVolume == uRendah)
  {
    OutputFuzzySuhu = "Lambat";
  }
  else if (kondisiSuhu == uDingin && kondisiVolume == uSedang)
  {
    OutputFuzzySuhu = "Lambat";
  }
  else if (kondisiSuhu == uDingin && kondisiVolume == uTinggi)
  {
    OutputFuzzySuhu = "Sedang";
  }
}

void basis_aturan_fuzzyPh()
{
  if (kondisiSuhu == uDingin && kondisiVolume == uRendah)
  {
    OutputFuzzySuhu = "Lambat";
  }
  else if (kondisiSuhu == uDingin && kondisiVolume == uSedang)
  {
    OutputFuzzySuhu = "Lambat";
  }
  else if (kondisiSuhu == uDingin && kondisiVolume == uTinggi)
  {
    OutputFuzzySuhu = "Sedang";
  }
}