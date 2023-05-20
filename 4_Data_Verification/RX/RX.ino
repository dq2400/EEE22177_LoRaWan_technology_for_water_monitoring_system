//wifi and clock
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//LoRa
#include <SPI.h>
#include <LoRa.h>

//OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Define Pins
#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET v1(14) v2(23)
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    915E6
#define LEDpin  2   // v1(2) v2(25)
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;
int rssi;

// Replace with your network credentials
char ssid[]           = ""; //SSID
const char password[] = ""; //Password

// Replace with your unique IFTTT URL resource
const char* resource = "/trigger/ht_readings/with/key/"; //IFTTT URL

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,10);
  display.print("Connecting to Wifi");
  display.display();
   
  // Initialize Serial Monitor
  Serial.begin(115200); //MUST ENSURE BAUD TIME MATCHES!!!!!!!!!!!!
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(28800); //Set offset time in seconds to adjust for your timezone GMT +8 = 28800

  
  // Initialize LoRa Module
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Receiver Callback");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,10);
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);


}



void loop() {
/*  timeClient.forceUpdate();
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("Time: ");
  Serial.println(timeStamp); */


  int packetSize = LoRa.parsePacket();
  if (packetSize) { 
    Serial.print("Received packet ");
    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
  
      //print RSSI of packet
      rssi = LoRa.packetRssi();
      Serial.print(" with RSSI ");    
      Serial.println(rssi);

      //reply
    LoRa.beginPacket();
      LoRa.print(LoRaData);
    LoRa.endPacket();
      
      }
   // Display information
   display.clearDisplay();
   display.setCursor(0,0);
   display.print("LoRa RECEIVER");
   display.setCursor(0,10);
   display.print("Received packet:");
   display.setCursor(0,20);
   display.print(LoRaData);
   display.setCursor(0,40);
   display.print("RSSI:");
   display.setCursor(30,40);
   display.print(rssi); 
   
   //get server time
   timeClient.forceUpdate();
   formattedDate = timeClient.getFormattedDate();
   // Extract date
   int splitT = formattedDate.indexOf("T");
   dayStamp = formattedDate.substring(0, splitT);
   // Extract time
   timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
   display.setCursor(0,50);
   display.print(dayStamp + " ");
   display.print(timeStamp);
   display.display(); 

   makeIFTTTRequest();
   
   }
}

void makeIFTTTRequest() {
  Serial.print("Connecting to "); 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
    Serial.println("Failed to connect...");
  }

  //Splitting the Data Received
  int index = LoRaData.indexOf('=');
  String HValue = LoRaData.substring(2 , 7);
  String TValue = LoRaData.substring(10, 15);
  Serial.print("Request resource: "); 
  Serial.println(resource);

  // Send data
  String jsonObject = String("{\"value1\" : \"" + LoRaData.substring(2,7) + "\",\"value2\" : \"" + LoRaData.substring(10,15)+ + "\",\"value3\" : \"" + rssi)+ "\"}";
  Serial.print(jsonObject);                  
  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server); 
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);
        
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
    Serial.println("No response...");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  
  Serial.println("\nclosing connection");
  client.stop(); 
}
