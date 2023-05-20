#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

/*
Transmitter
*/

int counter = 0;

void setup() {
  dht.begin();
  if (!LoRa.begin(915E6)) {
    while (1);
  }
}

void loop() {
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
  }

  LoRa.endPacket();

  //Sent packet

  counter++;
  delay(2000);
}
