//Used to toggle calibration mode on/off
#define CALPIN 3
//Which value is being calibrated, low for caltemp1.
#define MODEPIN 4

// Array of analog pins
static const uint8_t analog_pins[] = {A0,A1,A2,A3,A4,A5};
static const float   intercepts[]  = {0,0,0,0,0,0};
static const float   slopes[]      = {1,1,1,1,1,1};

//Current pin to sample
int pin = 0;
//Stores sample being read
int sample = 0;
//Used for temporarily storing the recorded values.
float value = -1;

//Auto-calibration variables.
//Lower temp to calibrate to
float caltemp1 = 0;
//Upper temp to calibrate to.
float caltemp2 = 100;
//Pin currently being calibrated
int calPin = 0;
//value read off caltemp1
float calVal1 = 0;
//value read off caltemp2
float calVal2 = 0;
//Whether in calibration or read mode.
bool calibrate = false;

void setup()
{
  Serial.begin(9600);
  pinMode(CALPIN, INPUT);
  pinMode(MODEPIN, INPUT);
}

void calibrate()
{
  
}

void loop()
{
  calibrate = digitalRead(CALPIN);
  if(calibrate)
  {
    calibrate();
  }
  else
  {
    // Loop over the 6 analog pins
    for(pin = 0; pin<6; pin++)
    {
      Serial.print(getTemp(),2);
      Serial.print("\t");
    }
    Serial.println();
  }
  delay(100);
}

float getTemp()
{
  value = 0;
  sample = 0;
  while (sample < 10) {
    value += analogRead(analog_pins[pin]);
    sample++;
    delay(1);
  }
  value /= 10;
  value = slopes[pin]*value + intercepts[pin];
  return value;
}

