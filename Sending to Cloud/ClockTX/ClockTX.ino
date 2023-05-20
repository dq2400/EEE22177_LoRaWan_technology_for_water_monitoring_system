#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"
#include <RTCZero.h> //Library for Internal RTC
#include <MKRWAN.h>
#include "ArduinoLowPower.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
RTCZero rtc;
/*
Transmitter
*/

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 20;
const byte hours = 01;

/* Change these values to set the current initial date */
const byte day = 6;
const byte month = 12;
const byte year = 22;

LoRaModem modem;

void setup() {
  dht.begin();
  if (!LoRa.begin(915E6)) {
    while (1);
  }
  rtc.begin(); // initialize RTC
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  for (int i=0; i < 15; i++) {
   pinMode(i, INPUT_PULLUP);
  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

void loop() {
  
  LowPower.deepSleep(5000);
  
  //Read Data
  int k = 1;
  float hic = 0;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    int k = 0;
    return;
  }
  else {
    float hic = dht.computeHeatIndex(t, h, false);
  }
  
  //Sends packet

  LoRa.beginPacket();
  if(k==0){
    LoRa.print("Reading error");
  }
  else{
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
  }

  LoRa.endPacket();

  //Sent packet
}


void LoRaprint2digits(int number) {
  if (number < 10) {
    LoRa.print("0"); // print a 0 before if the number is < than 10
  }
  LoRa.print(number);
}
