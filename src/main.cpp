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
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

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

LiquidCrystal_I2C lcd(0x27, 16, 2);

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


float uAnggota, uRendah, uSedang, uTinggi, N_suhu[3],N_ph[3],N_volume[3],
    uAsam, uNetral, uBasa, uDingin, uNormal, uPanas, Min_ph[9],Min_suhu[9];

float keluaranSuhu;
float coa_ph;
float keluaranPh;
float kondisiSuhu;
float kondisiPh;
float kondisiVolume;
float suhuArea;
String OutputFuzzySuhu;


byte Heart[8] = {
0b00000,
0b01010,
0b11111,
0b11111,
0b01110,
0b00100,
0b00000,
0b00000
};


void setup_wifi()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("SKRIPSHIT!!");
  lcd.setCursor(1,1);
  lcd.print("--ULUL AZMI--");
  delay(1000);
  lcd.clear();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  lcd.print("WIFI: ");
  lcd.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(" . ");
  }
  randomSeed(micros());
  lcd.setCursor(1,0);
  Serial.println("");
  Serial.println("Wifi Connected");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("CONNECTED");
  lcd.setCursor(1,1);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
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
      // client.subscribe("1710510160@stmikbumigora.ac.id/sensor/ph");
      // client.subscribe("1710510160@stmikbumigora.ac.id/sensor/suhu");
      // client.subscribe("1710510160@stmikbumigora.ac.id/sensor/levelair");
      // client.subscribe("1710510160@stmikbumigora.ac.id/sensor/suhuair");
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

void outputLCD(float msg1, float msg2, float msg3, float msg4){
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("PH:");
  lcd.print(msg1);
  lcd.setCursor(7,0);
  lcd.print("AIR:");
  lcd.print(msg2);
  lcd.setCursor(0,1);
  lcd.print("TMP:");
  lcd.print(msg3);
  lcd.setCursor(7,1);
  lcd.print("WTMP:");
  lcd.print(msg4);
}

///////////////////////////////////////

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

//fuzzyfikasi
void fuzzy()
{
  // int uAnggota;
  //tinggi air
  //angka 4 diganti pake variabel yg nampung air
  uAnggota = 0;
  hitung_anggota(1, distance, 0, 3, 6);
  uRendah = uAnggota;
  hitung_anggota(2, distance, 3, 6, 9);
  uSedang = uAnggota;
  hitung_anggota(3, distance, 6, 9, 9);
  uTinggi = uAnggota;

  //Ph
  uAnggota = 0;
  hitung_anggota(1, phValue, 0, 5, 7);
  uAsam = uAnggota;
  hitung_anggota(2, phValue, 5, 7, 9);
  uNetral = uAnggota;
  hitung_anggota(3, phValue, 7, 9, 14);
  uBasa = uAnggota;
  

  //suhu
  uAnggota = 0;
  hitung_anggota(1, suhuArea, 0, 25, 30);
  uDingin = uAnggota;
  hitung_anggota(2, suhuArea, 25, 30, 35);
  uNormal = uAnggota;
  hitung_anggota(3, suhuArea, 30, 35, 60);
  uPanas = uAnggota;


}

float fuzzy_setSuhu[3][3] = {
    {15, 15, 35},
    {15, 35, 50},
    {35, 50, 50},
};

float fuzzy_setPh[3][3] = {
    {1, 1, 2},
    {1, 2, 3},
    {2, 3, 3},
};



void defuzzyfikasi()
{

  float pembilangSuhu = 0, penyebutSuhu = 0,
        pembilangPh = 0, penyebutPh = 0, coa_suhu = 0, coa_ph=0;


  N_suhu [3]  = {};
  N_ph[3] = {};
  N_volume[3] = {};

  for (int set = 0; set < 9;)
  {
    for (int i = 0; i < 3; i++)
    {

      for (int j = 0; j < 3; j++)
      {

        //suhu
        float data_uSuhu[3] = {uDingin, uNormal, uPanas};
        N_suhu[i] = {data_uSuhu[i]};
        //ph
        float data_uPh[3] = {uAsam, uNetral, uBasa};
        N_ph[i] = {data_uPh[i]};

        //volume
        float data_uVolume[3] = {uRendah, uSedang, uTinggi};
        float N_volume[i] = {data_uVolume[i]};


        kondisiSuhu = max(N_suhu[i], kondisiSuhu);
        kondisiPh = max(N_ph[i], kondisiPh);
        kondisiVolume = max(N_volume[i], kondisiVolume);

        //coa
        //suhu
        Min_suhu[set] = {min(N_suhu[i], N_volume[j])};
        pembilangSuhu += Min_suhu[set] * fuzzy_setSuhu[i][j];
        penyebutSuhu += Min_suhu[set];

        //ph
        Min_ph[set] = {min(N_ph[i], N_volume[j])};
        pembilangPh += Min_ph[set] * fuzzy_setPh[i][j];
        penyebutPh += Min_ph[set];
        delay(5);
        set ++;
        
      }
    }
    //coa
    coa_suhu = pembilangSuhu / penyebutSuhu;
    keluaranSuhu = coa_suhu;

    coa_ph = pembilangPh / penyebutPh;
    keluaranPh = coa_ph;
    Serial.print("KELUARAN PH: ");
    Serial.println(keluaranPh);
    Serial.print("KELUARAN SUHU: ");
    Serial.println(keluaranSuhu);
  }
  
}


void basis_aturan_fuzzySuhu()
{
  if (kondisiSuhu == uDingin && kondisiVolume == uRendah)
  {
    OutputFuzzySuhu = "Lambat";
    Serial.println(OutputFuzzySuhu);
  }
  else if (kondisiSuhu == uDingin && kondisiVolume == uSedang)
  {
    OutputFuzzySuhu = "Lambat";
    Serial.println(OutputFuzzySuhu);
  }
  else if (kondisiSuhu == uDingin && kondisiVolume == uTinggi)
  {
    OutputFuzzySuhu = "Sedang";
    Serial.println(OutputFuzzySuhu);
  }
}

void basis_aturan_fuzzyPh()
{
  if (kondisiSuhu == uDingin && kondisiVolume == uRendah)
  {
    OutputFuzzySuhu = "Lambat";
    Serial.println(OutputFuzzySuhu);
  }
  else if (kondisiSuhu == uDingin && kondisiVolume == uSedang)
  {
    OutputFuzzySuhu = "Lambat";
    Serial.println(OutputFuzzySuhu);
  }
  else if (kondisiSuhu == uDingin && kondisiVolume == uTinggi)
  {
    OutputFuzzySuhu = "Sedang";
    Serial.println(OutputFuzzySuhu);
  }
}

//////////////////////////////////

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
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.createChar(0, Heart);
  // //lcd.begin(16,2);
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  lcd.print("AZMI");
  lcd.setCursor(4,0);
  lcd.write(0);
  lcd.setCursor(5,0);   //Move cursor to character 2 on line 1
  lcd.print("CYNDI");
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
    client.publish("1710510160@stmikbumigora.ac.id/sensor/ph", msg);
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
      suhuArea = event.temperature;
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
    outputLCD(phValue,distance,suhuArea,temperature);
    Serial.println("----------FUZZYYY---------");
    fuzzy();
    defuzzyfikasi();
    basis_aturan_fuzzyPh();
    basis_aturan_fuzzySuhu();
    Serial.println("-------------------------");

    //------------------------------------------------------------------------------
  }
  ph.calibration(voltage, temperature);
  
}


//sini

/////////////////////////////////////////////////////////////



