  /*
Project:  BME280 Sensor, анемометр, флюгер, отправка показаний на MQTT-брокер.  ESP8266 / NodeMCU
Author:   Artur Gorozinskiy
Date:     Created 02.02.2022
Version:  V1.1
Changelog:
OTA implementation 
 
Используемые библиотеки. (Tools -> manage libraries)
 - PubSubClient by Nick O'Leary V2.7.0  
 - Adafruit BME280 Library V2.0.1
 - Adafruit Unified Sensor V1.1.2
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community V2.6.3
Подключение BME280 Sensor:
BME280      NodeMCU
VCC         3.3V
GND         G
SCL         D1
SDA         D2
Подключение Анемометра:
VCC         3.3V
GND         G
Signal      D7
Подключение Флюгера:
VCC         3.3V 
Signal      A0   "VCC через резистор 10кОм на A0"
GND         G
*/
bool debug                          = true;    // Вывод информации в serial port

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "config.h"

const float SEA_LEVEL_PRESSURE_HPA = 59.05;
const int DELAY = 3000;
const int STARTUP_DELAY = 500;
const byte ventPin              = A0;     // PIN подключение лопастного датчика.
const byte AnemometrePin        = D7;     // PIN de connexion de l'anémomêtre.
unsigned int anemometreCnt      = 0;      // Инициализация счетчика.
unsigned long lastSendVent      = 0;      // Милли последней отправки (используется для получения реального интервала, а не желаемого значения).
unsigned long t_lastActionVent  = 0;      // enregistre le Time de la dernière intérogation des capteurs vent.
unsigned long t_lastRafaleVent  = 0;      // Enregistre le Time du dernier relevé de Rafale de vent.
unsigned int rafalecnt          = 0;      // Счетчик для расчета порывов ветра.
unsigned int anemometreOld      = 0;      // Сохранение показаний "анемометра" для расчета Rafale.

// Данные получены из этой библиотеки.
String wd = "other";                      // Направление ветра по сторонам света.
int angw  = 0;                            // Направление ветра по сторонам света.


#define INTERO_VENT 30                    // Valeur de l'intervale en secondes entre 2 relevés des capteurs Vent.


ICACHE_RAM_ATTR void cntAnemometre() {
  anemometreCnt++;
}

Adafruit_BME280 bme; // I2C
float temp = 0.0;
float hum = 0.0;
float pres = 0.0;
float diff = 1.0;

unsigned long delayTime;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_ota();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  analogRead(ventPin); 
  pinMode(AnemometrePin, INPUT_PULLUP);          // Montage PullUp avec Condensateur pour éviter l'éffet rebond.
  attachInterrupt(digitalPinToInterrupt(AnemometrePin), cntAnemometre, FALLING); // CHANGE: Déclenche de HIGH à LOW ou de LOW à HIGH - FALLING : HIGH à LOW - RISING : LOW à HIGH.
  lastSendVent = millis();
  while(!Serial);    // time to get serial running
    unsigned status;
    status = bme.begin(0x76, &Wire);   //I2C address is either 0x76 or 0x77
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        while (1);
    }

    
    delayTime = 1000;
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle(); 
    if (millis() - t_lastRafaleVent >= (5 * 1000)) {
    // On met à jour la valeur du dernier traitement de Rafale de vent à maintenant.
    t_lastRafaleVent = millis();
    
    // On a atteint l'interval souhaité, on exécute le traitement Vent.
    getRafale();
    
  }
  
  // On vérifie si l'intervale d'envoi des informations "Vent" sont atteintes. (120s)
  if (millis() - t_lastActionVent >= (INTERO_VENT * 1000)) {
    // On met à jour la valeur du dernier traitement de l'anémometre à maintenant.
    t_lastActionVent = millis();
    
    // On a atteint l'interval souhaité, on exécute le traitement Vent.
    getdirWind();
    getSendVitesseVent();
    getBME280Values();
  }
  
}
