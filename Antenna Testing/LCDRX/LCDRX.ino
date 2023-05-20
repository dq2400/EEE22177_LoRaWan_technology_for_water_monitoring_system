#include <LiquidCrystal.h>
#include <SPI.h>
#include <LoRa.h>

/*

  The circuit:
 * LCD RS pin to digital pin 4
 * LCD Enable pin to digital pin 5
 * LCD D4 pin to digital pin 3
 * LCD D5 pin to digital pin 2
 * LCD D6 pin to digital pin 1
 * LCD D7 pin to digital pin 0
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin

*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 4, en = 5, d4 = 3, d5 = 2, d6 = 1, d7 = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Initialising...");
  // initialize the serial communications:
  Serial.begin(9600);
  Serial.println("LoRa RX");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  lcd.setCursor(0,1);
  lcd.print("Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if(packetSize) {
    lcd.clear();
    lcd.setCursor(0,0);
        
    while (LoRa.available()) {
      lcd.print((char)LoRa.read());
    }

    lcd.setCursor(0,1);
    lcd.print(LoRa.packetRssi());
  }
}
