#include <ADC.h>

ADC *adc = new ADC(); // adc object
int pga = 4;
int data = 0;
char reading = 0;
int prevData = 0;
bool badData = 0;
float DCVoltage = 0;
float ACVoltage = 0;
float i = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  //  pinMode(A10, INPUT); //Diff Channel 0 Positive
  //  pinMode(A11, INPUT); //Diff Channel 0 Negative
  //
  //  /********DC VOLTAGE (ADC CHANNEL 0) ***************/
  //  adc->setReference(ADC_REF_1V2, ADC_0);
  //  adc->setAveraging(32); // set number of averages
  //  adc->setResolution(12); // set bits of resolution
  //  adc->enablePGA(pga);
  //  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  //  // see the documentation for more information
  //  // additionally the conversion speed can also be ADC_ADACK_2_4, ADC_ADACK_4_0, ADC_ADACK_5_2 and ADC_ADACK_6_2,
  //  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  //  adc->setConversionSpeed(ADC_LOW_SPEED); // change the conversion speed
  //  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  //  adc->setSamplingSpeed(ADC_LOW_SPEED); // change the sampling speed
  //  adc->startContinuousDifferential(A10, A11, ADC_0);
  //
  //  /********AC VOLTAGE (ADC CHANNEL 1) ***************/
  //  adc->setReference(ADC_REF_1V2, ADC_1);
  //  adc->setAveraging(32, ADC_1); // set number of averages
  //  adc->setResolution(12, ADC_1); // set bits of resolution
  //  adc->enablePGA(pga, ADC_1);
  //  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  //  // see the documentation for more information
  //  // additionally the conversion speed can also be ADC_ADACK_2_4, ADC_ADACK_4_0, ADC_ADACK_5_2 and ADC_ADACK_6_2,
  //  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  //  adc->setConversionSpeed(ADC_LOW_SPEED, ADC_1); // change the conversion speed
  //  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  //  adc->setSamplingSpeed(ADC_LOW_SPEED, ADC_1); // change the sampling speed
  //  // always call the compare functions after changing the resolution!
  //  adc->enableCompare(1.2/1.2*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
  //  adc->startContinuousDifferential(A12, A13, ADC_1);

  delay(2000);

  //  DCVoltage = (float)adc->analogReadContinuous(ADC_0)/(4096.0)*1.2/pga;
  //  ACVoltage = (float)adc->analogReadContinuous(ADC_1)/(4096.0)*1.2/pga;
  //  Serial.write('%');
  //  printVoltage(DCVoltage);
  //  Serial.write('~');
  //  printVoltage(ACVoltage);
}

void loop() {
  // put your main code here, to run repeatedly:
    serialComm();
  //  voltageReadWrite();
//  i = i + .1;
//  Serial.write('%');
//
//  if (i < 10) {
//    Serial.print(i, 3);
//  }
//  else {
//    Serial.print(i, 2);
//  }
//  delay(1000);
//  i = i + .1;
//  Serial.write('~');
//  if (i < 10) {
//    Serial.print(i, 3);
//  }
//  else {
//    Serial.print(i, 2);
//  }
//  delay(1000);
//  i = i + .1;
//  if (i < 10) {
//    Serial.print(i, 3);
//  }
//  else {
//    Serial.print(i, 2);
//  }

  //5 milliseconds is needed to seperate serial packets
//  delay(1000);

}



void serialComm() {
  if (Serial1.available() > 0) {
    data = Serial1.read();

    if (reading == 1) {
      if (data == 48 && prevData == 48) {
        badData = 1;
      }
      else if (data == 49 && prevData == 48) {
        badData = 1;
      }
      else {
        badData = 0;
        Serial.write(prevData);
        Serial.write(data);
      }
    }
    reading++;
    if (data == 9) {
      if (badData == 0) {
        Serial.write(data);
      }
      reading = 0;
    }
    prevData = data;
  }
}

void printVoltage(float voltage) {
  if (voltage < 10) {
    Serial.println(voltage, 3);//used to be 2
  }
  else {
    Serial.println(voltage, 2);//used to be 1
  }
}

void voltageReadWrite() {
  bool pgaChange = 0;
  float changeThres = 0.001;
  static float prevDC;
  static float prevAC;
  DCVoltage = (float)adc->analogReadContinuous(ADC_0) / (4096.0) * 1.2 / pga;
  ACVoltage = (float)adc->analogReadContinuous(ADC_1) / (4096.0) * 1.2 / pga;
  if (abs(prevDC - DCVoltage) > changeThres) {
    Serial.write('%');
    printVoltage(DCVoltage);
  }
  //  if (abs(prevAC - ACVoltage) > changeThres) {
  //    Serial.write('~');
  //    printVoltage(ACVoltage);
  //  }
  prevDC = DCVoltage;
  prevAC = ACVoltage;
}
