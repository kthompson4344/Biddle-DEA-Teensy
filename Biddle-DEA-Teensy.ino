//This code sends takes serial PD data from the FPGA, analog voltages from the biddle, and sends them via serial over usb to labview.

#include <ADC.h>

//TEST 921600 BAUD
//IF IT FAILS, SEND ONLY VOLTAGE WITH THIS

ADC *adc = new ADC(); // adc object
int DCpga = 4;//can only be 1,2,4,8,16,32,64
int ACpga = 4;//can only be 1,2,4,8,16,32,64
int data = 0;
char reading = 0;
int prevData = 0;
bool badData = 0;
float DCVoltage = 0;
float ACVoltage = 0;
float i = 0;
bool needReading = true;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Serial1.begin(921600);

  pinMode(A10, INPUT); //Diff Channel 0 Positive
  pinMode(A11, INPUT); //Diff Channel 0 Negative
  pinMode(A12, INPUT); //Diff Channel 1 Positive
  pinMode(A13, INPUT); //Diff Channel 1 Negative

  /********DC VOLTAGE (ADC CHANNEL 0) ***************/
  adc->setReference(ADC_REF_1V2, ADC_0);
  adc->setAveraging(32, ADC_0); // set number of averages
  adc->setResolution(12, ADC_0); // set bits of resolution
  adc->enablePGA(DCpga, ADC_0);
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  // see the documentation for more information
  // additionally the conversion speed can also be ADC_ADACK_2_4, ADC_ADACK_4_0, ADC_ADACK_5_2 and ADC_ADACK_6_2,
  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  adc->setConversionSpeed(ADC_LOW_SPEED, ADC_0); // change the conversion speed
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_LOW_SPEED, ADC_0); // change the sampling speed
  //  adc->startContinuousDifferential(A10, A11, ADC_0);

  /********AC VOLTAGE (ADC CHANNEL 1) ***************/
  adc->setReference(ADC_REF_1V2, ADC_1);
  adc->setAveraging(32, ADC_1); // set number of averages
  adc->setResolution(12, ADC_1); // set bits of resolution
  adc->enablePGA(ACpga, ADC_1);
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  // see the documentation for more information
  // additionally the conversion speed can also be ADC_ADACK_2_4, ADC_ADACK_4_0, ADC_ADACK_5_2 and ADC_ADACK_6_2,
  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  adc->setConversionSpeed(ADC_LOW_SPEED, ADC_1); // change the conversion speed
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_LOW_SPEED, ADC_1); // change the sampling speed
  // always call the compare functions after changing the resolution!
  //  adc->enableCompare(1.2 / 1.2 * adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
  //  adc->startContinuousDifferential(A12, A13, ADC_1);

  //delay(1000);
    while (Serial.available() > 0) {
      Serial.read();
    }
    
    while (Serial.available() == 0) {
    }

    if (Serial.read() != '@') {
      while(1);
    }
    
    DCVoltage = (float)adc->analogReadDifferential(A10,A11,ADC_0) / (4096.0) * 1.2 / DCpga;
    ACVoltage = -(float)adc->analogReadDifferential(A12,A13,ADC_1) / (4096.0) * 1.2 / ACpga;//Wired Backwords
    Serial.write('%');
    printVoltage(DCVoltage);
    Serial.write('~');
    printVoltage(ACVoltage);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    Serial.read();
    DCVoltage = (float)adc->analogReadDifferential(A10,A11,ADC_0) / (4096.0) * 1.2 / DCpga;
    ACVoltage = -(float)adc->analogReadDifferential(A12,A13,ADC_1) / (4096.0) * 1.2 / ACpga;//Wired Backwords
    Serial.write('%');
    printVoltage(DCVoltage);
    Serial.write('~');
    printVoltage(ACVoltage);
  
  }
  voltageReadWrite();
  delay(5);
  //5 milliseconds is needed to seperate serial packets
}

void printVoltage(float voltage) {
  if (voltage*100 < 10) {
    Serial.println(voltage*100, 2);
  }
  else {
    Serial.println(voltage*100, 1);
  }
}

void voltageReadWrite() {
  float changeThres = 0.001;
  static float prevDC;
  static float prevAC;
  DCVoltage = (float)adc->analogReadDifferential(A10, A11, ADC_0) / (4096.0) * 1.2 / DCpga;
  ACVoltage = -(float)adc->analogReadDifferential(A12, A13, ADC_1) / (4096.0) * 1.2 / ACpga; //Wired Backwards
  gainChange(ACVoltage, true);
  gainChange(DCVoltage, false);
  if (abs(prevDC - DCVoltage) > changeThres) {
    Serial.write('%');
    printVoltage(DCVoltage);
  }
  if (abs(prevAC - ACVoltage) > changeThres) {
    Serial.write('~');
    printVoltage(ACVoltage);
  }
  prevDC = DCVoltage;
  prevAC = ACVoltage;
}

void gainChange(float voltage, bool AC) {
  int pga;
  if (voltage < 0.0187) {
    pga = 64;
  }
  else if (voltage < 0.037) {
    pga = 32;
  }
  else if (voltage < 0.07) {
    pga = 16;
  }
  else if (voltage < 0.1) {
    pga = 8;
  }
  else if (voltage < 0.29) {
    pga = 4;
  }
  else {
    pga = 2;
  }
  if (AC == true) {
    ACpga = pga;
    adc->enablePGA(ACpga, ADC_1);
  }
  else {
    DCpga = pga;
    adc->enablePGA(DCpga, ADC_0);
  }
}




