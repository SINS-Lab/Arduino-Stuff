// pins for high precision output
#define PINOCOARSE 3
#define PINOFINE 5

float voltageOut_coarse_prev = 0.0;
float voltageOut_coarse = 0.0;
float voltageOut_fine_prev = 0.0;
float voltageOut_fine = 0.0;
String serial_in;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PINOFINE, OUTPUT);
  pinMode(PINOCOARSE, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() >= 0)
  {
    serial_in = Serial.readString();
    if(serial_in!="")
    {
      //Echo back what we got
      Serial.println(serial_in);
      char buf[20];
      char *pnt = buf;
      char *tmp;
      serial_in.toCharArray(buf, sizeof(buf));
      int n = 0;
      while ((tmp = strtok_r(pnt, " ", &pnt)) != NULL)
      {
        if(tmp!="")
        {
          //coarse
          if(n==0)
          {
            voltageOut_coarse = ((String)tmp).toFloat();
          }
          //fine
          if(n==1)
          {
            voltageOut_fine = ((String)tmp).toFloat();
          }
          Serial.println((String)"Value: "+tmp);
          Serial.println((String)"Index: "+n);
        }
        n++;
      }
    }
  }

  if(voltageOut_coarse != voltageOut_coarse_prev)
  {
    analogWrite(PINOCOARSE, (int)voltageOut_coarse);
    voltageOut_coarse_prev = voltageOut_coarse;
  }
  if(voltageOut_fine != voltageOut_fine_prev)
  {
    analogWrite(PINOFINE, (int)voltageOut_fine);
    voltageOut_fine_prev = voltageOut_fine;
  }
  
  delay(100);
}
