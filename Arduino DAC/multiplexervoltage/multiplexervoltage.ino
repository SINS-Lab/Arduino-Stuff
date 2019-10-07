/*
 * Each voltage supply provides a different maximum output voltage. The variable programmedVolt was designed based on a high voltage supply with a maximum of 7000V
 * Users reusing this code for other voltage supplies will have to edit this variable to their own preferences.
 * To include more than two voltage supplies, one must also define the correct number of pins and add appropriate logic statements to the loop() below.
 * 
 * This code was written for an AD7849BN 16-bit Multiplying DAC controlled by an Arduino UNO.
 * 
 */

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

#define syncbar 9

//function for setting output voltage
void setVoltage16(int dacpin, int16_t mV) {
  SPI.beginTransaction(SPISettings(115200, MSBFIRST, SPI_MODE0));
  digitalWrite(dacpin, LOW);
  SPI.transfer16(mV);
  digitalWrite(dacpin, HIGH);
  SPI.endTransaction();

}

void setup() {
  //defining signal pins on the Arduino UNO
  pinMode(syncbar, OUTPUT);
  SPI.begin();
  Serial.begin(9600);
}


String incomingString;
int zeroed = 1;
float num;
int prog;

void loop() 
{
  if (zeroed)
  {
    delay(1000);
    //Attempt to set the voltage
    setVoltage16(syncbar, 0);
    //Delay and do this again, just to make sure it is set.
    delay(10);
    setVoltage16(syncbar, 0);
    zeroed = 0;
  }

  if (Serial.available() >= 0) 
  {
    incomingString = Serial.readString();
    //No message, just skip
    if (incomingString != "") 
    {
      Serial.println(incomingString);  
      num = incomingString.toFloat();
      Serial.println((String)"Value: " + num);
      num /= 10.0;
      prog = (int)(num * 32767);
      Serial.println((String)"Prog: " + prog);
      //Attempt to set the voltage
      setVoltage16(syncbar, prog);
      //Delay and do this again, just to make sure it is set.
      delay(10);
      setVoltage16(syncbar, prog);
    }
  }
  delay(10);
}
