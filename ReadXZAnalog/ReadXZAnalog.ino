const int analogInPinX = A0;
const int analogInPinZ = A1;

int sensorValueX = 0;
int sensorValueZ = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValueX = analogRead(analogInPinX);
  sensorValueZ = analogRead(analogInPinZ);

    // print the results to the Serial Monitor:
  //Serial.print("X = ");
  Serial.print(sensorValueX);
  Serial.print(",");
  //Serial.print("Z = ");
  Serial.println(sensorValueZ);

  // Allow 2 ms or so for the ADC to reset
  delay(2);
}
