/*Transmitter*/
#include <SPI.h>
#include <LoRa.h> // LoRa Communication
#include <RTCZero.h> //Library for Internal RTC
#include <MKRWAN.h>
#include "ArduinoLowPower.h"

#include <SD.h> //SD card

#include <Wire.h> //include Wire.h library
#include "RTClib.h" //include Adafruit RTC library

#include <OneWire.h>
#include <DallasTemperature.h>

/*Temperature Sensor*/
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);
float tempDS = 0;

/*Water Sensor*/
#include <SDI12.h>  //library for OTT PLS
#define SERIAL_BAUD 9600
#define DATA_PIN 5 // Data wire is plugged into digital pin 5 on the Arduino
SDI12 mySDI12(DATA_PIN);
String SDI12data;
String RealRefLevel;
String Temperature;

//SDI 12 commands
#define myComAdress   "?!"    //ask the adress of the probe
#define myComId       "0I!"   //ask the identification
#define myComMeasure  "0M!"   //ask to start a measurement
#define myComSend     "0D0!"  //ask to send the measure
#define myComSetUnitCM  "0OSU+0!"  //set the unit
#define myComUnit     "0OSU!" //ask for the unit of the measure, +0=m, +1=cm, +2=ft, +3=mbar, +4=psi

/*RTC*/
RTC_DS3231 rtc; //Make a RTC DS3231 object

/*Set Date/time*/
int year;
int month;
int day;
int hour;
int minute;
int second;

LoRaModem modem;

/*Store into SD*/
const String codeVersion = "Mar 2023"; //version of the code
const String stationName = "NIE Rain Garden";
const String stationNumber = "Station 1";
uint8_t countSave = 0; // index for saving data to SD card
/*for SD Card*/
const char* SAVE_DATA_FILE_NAME = "DATA.CSV";
String DATA_Header = "Station Name, Station Number, Count Number, DS3231 (UTC+2), KIT Raw Data, PLS Raw Data, DS18B20 Temperature (Celsius),Code Version";
File SaveDATA;
int SDflag = 0;
const int chipSelect = 4;

/*ADS Module*/
#include <Adafruit_ADS1X15.h> // from; https://learn.adafruit.com/adafruit-4-channel-adc-breakouts/arduino-code
Adafruit_ADS1115 ads1115; /*Use this for the 16-bit version */
int KITrawData = 0;

/*Storage arrays*/
#define KITnos 10   //number of readings
#define DELAY 30000 //time between readings
int KITarr[KITnos];

/*OK NOW WE ARE FINALLY READY TO WRITE CODE*/

void setup() {
  rtc.begin(); // initialize RTC
  ads1115.begin(0x49); // ADS
  sensors.begin(); // Temperature
  //remove pins for power saving
/*  for (int i=0; i < 10; i++) {
     pinMode(i, INPUT_PULLUP);
    }*/
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(ONE_WIRE_BUS, INPUT);
  pinMode(DATA_PIN, INPUT);

  //Serial.begin(19200);
  /*while (!Serial) {
    ;
  } */
  //Serial.println("Begin");

}

void loop() {
//  USBDevice.detach(); //double click reset to allow update
  
  //Temperature Sensor DS18B20
  sensors.requestTemperatures();
  //Serial.println("DA18A");
  tempDS = sensors.getTempCByIndex(0);
  //Serial.println("DA18B");

  /*Read SDI*/
  mySDI12.begin();
  delay(2000);
  SDI12data = measREFsensor();
  mySDI12.end();
  
  //Set Date/Time DS3231
  setdatetime();

  //Read Low Cost Water Level Sensor Data ADS1115
  for(int i=0; i<KITnos; i++){
    KITarr[i] = ads1115.readADC_SingleEnded(0);
    LowPower.deepSleep(DELAY);
  }
  
  //Save to SD
  saveSD();
  
//  LowPower.deepSleep(5000); //sleep for x milisec
}


/*Functions*/

String print2digits(int number) { //print correctly dates
  String numback = "";
  if (number < 10) {
    numback = "0"; // print a 0 before if the number is < than 10
  }
  numback = numback + String(number);
  return numback;
}

void saveDATAln(String data) {
  SDflag = 0;
  SaveDATA = SD.open(SAVE_DATA_FILE_NAME, FILE_WRITE);
  if (SaveDATA.size() == 0) {
    SaveDATA.println(DATA_Header);
    SaveDATA.flush();
  }
  if (SaveDATA) {
    SaveDATA.println(data);
    for (int i=0; i<10; i++){
      SaveDATA.println(KITarr[i]);
    }
    SaveDATA.close();
    SDflag = 1;
  }
  else
  {
    SDflag = 0;
  }
}

String measREFsensor(){
  delay(300);      
  mySDI12.sendCommand(myComMeasure);
  while(mySDI12.available()){ mySDI12.read();}    
  delay(1000);    
  mySDI12.clearBuffer();   
  delay(500);
  mySDI12.sendCommand(myComSend);  
  delay(50);
  String rawdata = readsensor();
  mySDI12.clearBuffer();   
  delay(50); 
  
  //Decoding string sent from probe  
  int p=0;
  int pos[2];
  for (int z=0 ; z<rawdata.length() ; z++)  {
    char u = rawdata.charAt(z);
    if (u == '.') {
      pos[p] = z ;
      p++; 
    } 
  }    

  RealRefLevel = rawdata.substring(pos[0]-1,pos[0]+4);  //Water level from the reference sensor (without correction)
  Temperature = rawdata.substring(pos[1]-2,pos[1]+2); //Water temperature from the reference sensor
  return rawdata;                 
}

String readsensor() {
  String sdiResponse = "";
  delay(30);
  while(mySDI12.available()){    
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r')) { sdiResponse += c; delay(7.5); }  //must be more than 7.5ms
  }
  return sdiResponse;
}

String allSave() {
  String SendKITarrOUT;
  for(int i=0; i<KITnos; i++){
    SendKITarrOUT = SendKITarrOUT + "," + String(KITarr[i]);
  }
  String allData =
    stationName
    + "," + stationNumber
    + "," + String(countSave)
    + "," + print2digits(year)
    + "-" + print2digits(month)
    + "-" + print2digits(day)
    + " " + print2digits(hour)
    + ":" + print2digits(minute)
    + ":" + print2digits(second)
    + SendKITarrOUT
    + "," + String(RealRefLevel)
    + "," + String(Temperature)
//    + "," + String(tempDS)
    + "," + codeVersion;
  return (allData);
}

void setdatetime(){
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
}

void saveSD(){
  SD.begin(chipSelect);
  saveDATAln(allSave());
  countSave = countSave + 1;
  SD.end();
}
