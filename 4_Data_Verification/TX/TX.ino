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
/*Transmitter*/

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
  Serial.begin(115200);
  Serial.print("ok");
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
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);  
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(3000);
}

void loop() {
  LowPower.deepSleep(5000);
  
  pinMode(DHTPIN,INPUT);
  //Read Sensor Data
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  String Checker = "Null";
  String Data = LoRaPrintThis(h,t);
while(Checker != LoRaPrintThis(h,t)) //spam send until checker is all good
{

  //Sends packet
  LoRa.beginPacket();
    LoRa.print(Data);
  LoRa.endPacket();
  //Sent packet

  int packetSize = LoRa.parsePacket();
  int j = 0;
  while(!packetSize && j<1000){
    j++;
    packetSize = LoRa.parsePacket();
  }
  
  if (packetSize) { 
    Serial.print("Received packet ");
    //read packet
    int i = 0;
    while (LoRa.available() && i< 1000) {
      Checker = LoRa.readString();
      }
    }
  }
}

void LoRaPrint2Digits(int number) {
  if (number < 10) {
    LoRa.print("0"); // print a 0 before if the number is < than 10
  }
  LoRa.print(number);
}

String LoRaPrintThis(float h,float t){
  String data = "H=" + String(h) + " T=" + String(t) + "\n" + String(rtc.getHours()) + ":" + String(rtc.getMinutes()) + ":" + String(rtc.getSeconds());
  return data;
}
