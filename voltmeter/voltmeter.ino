/**
 * This is a high-precision voltmeter made by combining two analog inputs
 * for a coarse and a fine measurement.  It uses a PWM output for a feedback
 * loop for determining the fine voltage.
 * 
 * TODO find a way to include the circuit diagram for the associated circuitry here.
 */


// number of analog samples to take per reading
#define NUM_SAMPLES 100
// Pin for coarse measurements
#define PINI1 A0
// Pin for fine measurements
#define PINI2 A1
// Pin for feedback input voltage
#define PINO1 3

// pins for high precision output
#define PINOCOARSE 9
#define PINOFINE 10

// Calibration factor for coarse input
// This should be set based on the 5V rail of the arduino
#define CALSCALE_COARSE 5.0
#define CALSHIFT_COARSE 0 // Constant offset for coarse scale
// Calibration factor for fine input
// This should be set by checking relative offset between coarse
// and an external voltmeter
#define CALSCALE_FINE 0.0805
#define CALSHIFT_FINE 0 // constant offset for fine scale

// This should be set based on the 5V rail of the arduino
#define OUTPUTSCALE 5.0

#define coarse_bitdrop 4//This should be at least 4

unsigned char sample_count = 0; // current sample number

long sumCoarse = 0;         // sum of coarse samples
float voltageCoarse = 0;    // stores calculated values for coarse
long sumFine = 0;           // sum of fine samples
float voltageFine = 0.0;    // stores calculated values for fine
float voltageIn = 0.0;      // stores the calulated input voltage

// Values for use with feedback analog outputs, these should be
// Calibrated such that it matches the voltage in.
float voltageOut_prev = 0.0;
float voltageOut = 2.5;
int outputCoarse = 0;
int outputFine = 0;
float temp = 0.0;

void setup()
{
    Serial.begin(9600);
    pinMode(PINO1, OUTPUT);
    pinMode(PINOFINE, OUTPUT);
    pinMode(PINOCOARSE, OUTPUT);
}

/** 
 *  Checks serial for any desired input voltages.
 *  if one is found, voltageOut is set.
 */
void readSerial()
{
  //For now, we just set desired voltage out to be same as the read voltage.
  voltageOut = voltageIn;
}

/**
 * Reads NUM_SAMPLES samples from the coarse and fine
 * inputs, and sums them, for later use in averaging.
 */
void readAnalogIn()
{
    // take a number of analog samples and add them up
    while (sample_count < NUM_SAMPLES) {
        sumCoarse += analogRead(PINI1);
        sumFine += analogRead(PINI2);
        sample_count++;
        delay(1);
    }
}

/**
 * Calculates the coarse and fine voltages from the
 * corresponding sums read in readAnalogIn, then
 * outputs the coarse value on analogWrite
 */
void processAnalogIn()
{
    // calculate the average values from the read inputs.
    voltageCoarse = (((float)sumCoarse / (float)NUM_SAMPLES));
    voltageFine = (((float)sumFine / (float)NUM_SAMPLES));

    /*convert to scaled voltages
     * 
     * coarse voltage is converted to down a few bits of precision 
     * to increase relative size of the fine voltage input,
     * this is to reduce the relative ripple in the feedback loop
     */
    voltageCoarse = ((int)voltageCoarse/coarse_bitdrop);
    
    // Output adjusted coarse voltage for use in feedback loop.
    analogWrite(PINO1, voltageCoarse * coarse_bitdrop / 4);
    //calculate the coarse voltage.
    voltageCoarse = (voltageCoarse * coarse_bitdrop * CALSCALE_COARSE / 1024.0) + CALSHIFT_COARSE;
    
    //Voltage 2 is read normally, scaled accordingly, for the summed measurement
    voltageFine = (voltageFine * CALSCALE_FINE / 1024.0) + CALSHIFT_FINE;
    // Sum the coarse and fine voltages for total voltage
    voltageIn = voltageFine + voltageCoarse;
}

void processAnalogOut()
{
    //If no new voltage to output, nothing is needed to be done.
    if(voltageOut == voltageOut_prev)
    {
      return;
    }
    
    temp = voltageOut * 256.0 / OUTPUTSCALE;
    outputCoarse =(int)(temp);
    outputFine = (int)((temp - outputCoarse) * 256.0);

    /*Write the fine/coarse voltages, assume the attached circuitry
      Will handle combining them properly.*/
    analogWrite(PINOCOARSE, outputCoarse);
    analogWrite(PINOFINE, outputFine);

    //Update previous set voltage.
    voltageOut_prev = voltageOut;
}

void reset()
{
    sample_count = 0;
    sumCoarse = 0;
    sumFine = 0;
}

void writeSerial()
{
    Serial.print((((float)sumCoarse / (float)NUM_SAMPLES)));
    Serial.print("\t");
    Serial.print(voltageCoarse, 5);
    Serial.print("\t");
    Serial.print((((float)sumFine / (float)NUM_SAMPLES)));
    Serial.print("\t");
    Serial.print(voltageFine, 5);
    Serial.print("\t");
    Serial.print(voltageIn, 5);
    Serial.println("");
}

void loop()
{
    //Read the analog inputs
    readAnalogIn();

    //Process the values.
    processAnalogIn();

    //Output serial values
    writeSerial();

    //Read serial values for output
    readSerial();

    //process values read and apply appropriate outputs.
    processAnalogOut();

    //Reset analog read values for next loop
    reset();
    delay(100);
}
