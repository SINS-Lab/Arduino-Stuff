#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

#define AD  9

int16_t counter = 0x0000;

void setVoltage16(int dacpin, int16_t mV) {       //function for setting output voltage

  SPI.beginTransaction(SPISettings(115200, MSBFIRST, SPI_MODE0));
  digitalWrite(dacpin, LOW);
  SPI.transfer16(mV);
  digitalWrite(dacpin, HIGH);
  SPI.endTransaction();

}

void setup() {

  pinMode(AD, OUTPUT);
  SPI.begin();
  Serial.begin(115200);
}

int16_t programmedVolt;



void loop() {
  int incomingTotal = 0;
  while (incomingTotal <= 7000) {
    programmedVolt = (int16_t)(incomingTotal / 7000.0 * 0xFFFF); //scaling input voltage, changing input value to HEX
    setVoltage16(AD, programmedVolt);                           //setting voltage to that level
    incomingTotal += 10;
    Serial.println(incomingTotal);
    delay(100);
  }

}
