#include <Servo.h>

// servo object for the chopper
Servo servo;

//Pin numbers to use.
const int SERVOPIN = 2;
const int CONTROLPIN = 3;
const int LEDPIN = 4;

// Set point angles for on/off
const int ANGLEON = 60;
const int ANGLEOFF = 80;

// Variables to store position of chopper
int pos = ANGLEOFF;
int lastPos = 0;
// Variable for control signal
int controlState = 0;

void setup() {
  //Set up servo and IO pins
  servo.attach(SERVOPIN);
  pinMode(CONTROLPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  //Read control pin
  controlState = digitalRead(CONTROLPIN);
  //If High, set to ANGLEON, otherwise set to ANGLEOFF
  if(controlState==HIGH) 
  {
    pos = ANGLEON;
  }
  else 
  { 
    pos = ANGLEOFF;
  }

  // If Position has changed, move servo to new pos
  if(lastPos!=pos) 
  {
    servo.write(pos);
    // Set LED on/off based on where servo moved to
    if(pos==ANGLEON)
    {
      digitalWrite(LEDPIN, HIGH);
    }
    else
    {
      digitalWrite(LEDPIN, LOW);
    }
  }
  //Update last pos value
  lastPos = pos;
  //wait 50ms for next loop iteration
  delay(50);
}
