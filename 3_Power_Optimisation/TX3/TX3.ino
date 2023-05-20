#include "ArduinoLowPower.h"
#include <LoRa.h>
#include <MKRWAN.h>
#include "DHT.h"
#include <RTCZero.h> //Library for Internal RTC
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
LoRaModem modem;
RTCZero rtc;
DHT dht(DHTPIN, DHTTYPE);


/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 20;
const byte hours = 01;

/* Change these values to set the current initial date */
const byte day = 6;
const byte month = 12;
const byte year = 22;



void setup() {
  for (int i=0; i < 15; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  rtc.begin(); // initialize RTC
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  dht.begin();
}

void loop() {
    USBDevice.detach(); //double click reset to allow update

  pinMode(DHTPIN,INPUT);
  
    LowPower.deepSleep(8000);
    

  float h = dht.readHumidity();
  float t = dht.readTemperature();
/*
  int k = 0;
  while (k == 0)
  {
    if (isnan(h) || isnan(t)) {
      h = dht.readHumidity();
      t = dht.readTemperature();
      return;
    }
    k = 1;
  }
*/
  LoRa.begin(915E6);
  LoRa.beginPacket();
    LoRa.print("H=");
    LoRa.print(h);
    LoRa.print(" T=");
    LoRa.print(t);
    LoRa.println();
    LoRaprint2digits(rtc.getHours()); //retrieve hours
    LoRa.print(":");
    LoRaprint2digits(rtc.getMinutes()); //retrieve minutes
    LoRa.print(":");
    LoRaprint2digits(rtc.getSeconds()); //retrieve seconds
  LoRa.endPacket();
  LoRa.end();

  digitalWrite(LORA_IRQ_DUMB, LOW);
  digitalWrite(LORA_RESET, LOW);
  digitalWrite(LORA_BOOT0, LOW);
 }
 
void LoRaprint2digits(int number) {
  if (number < 10) {
    LoRa.print("0"); // print a 0 before if the number is < than 10
  }
  LoRa.print(number);
}
 //idle current: 390mA
 //sending curret: 17.6mA

 //ource code: https://www.thethingsnetwork.org/forum/t/mkrwan1300-high-current-in-sleep-mode/13749/49
