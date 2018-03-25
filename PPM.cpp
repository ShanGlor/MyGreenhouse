#include "PPM.h"
#include "Settings.h"
#include <ResponsiveAnalogRead.h>
#include <Vcc.h>

int ppmR1 = PPM_RESISTOR;
int ppmRa = 25;
float ppmTempFinish = 0;
float ppmTempStart = 0;
double lastEC = 0;
long int lastPPM = 0;
double ppmK = 0;
float PPMconversion = PPM_EC_CONVERSION;

const float VccCorrection = VCC_VOLTAGE_READ/VCC_VOLTAGE_REPORTED;
Vcc vcc(VccCorrection);
ResponsiveAnalogRead analogPpm(PPM_SENSE_PIN, false);


void readPPMValues(double temp){
  ppmR1 = (ppmR1 + ppmRa);
  Serial.print(ppmK);
  if (ppmK <= 0){
    calibratePPM(temp);
  }
  
  float EC = 0;
  float EC25 = 0;
  long PPM = 0;
  float raw = 0;
  float Vin = vcc.Read_Volts();
  float Vdrop = 0;
  float Rc = 0;
  float val = 0;
  digitalWrite(PPM_POWER_PIN, HIGH);
  analogPpm.update();
  raw = analogPpm.getValue();
  digitalWrite(PPM_POWER_PIN, LOW);

  Vdrop = ((Vin * raw) / 1024.0);
  Rc =(Vdrop * ppmR1) / (Vin - Vdrop);
  Rc = Rc - ppmRa;
  EC = 1000 / (Rc * ppmK);

  EC25 = EC / (1 + (PPM_TEMP_COEF * (24 - 25.0)));

  PPM = EC25 * PPMconversion * 1000;

//  if (abs(lastPPM - PPM) > PPM_REPORT_THRESHOLD && reportTimer > 0){
//    send(msgPpm.set(PPM));
//  }
  
  lastPPM = PPM;
  lastEC = EC25;
    
}

// Calibrate EC sensor
void calibratePPM(double temp){
  ppmR1 = (ppmR1 + ppmRa);
  Serial.print("****** Calibrating Probe ******");
  float EC = 0;
  float EC25 = 0;
  long PPM = 0;
  float raw = 0;
  float Vin = vcc.Read_Volts();
  float Vdrop = 0;
  float Rc = 0;
  float K = 0;
  
  int i=1;
  float buffer = 0;
   
  while(i<=10){
    Serial.println(10-i);
    digitalWrite(PPM_POWER_PIN, HIGH);
    delay(10);
    analogPpm.update();
    raw = analogPpm.getValue();
    digitalWrite(PPM_POWER_PIN, LOW);
    buffer = buffer + raw;
    i++;
    delay(500);
  };
  raw=(buffer/10);

  EC = PPM_CALIBRATION * (1 + (PPM_TEMP_COEF * (24 - 25.0))) ;

  Vdrop = ((Vin * raw) / 1024.0);
  Rc = (Vdrop * ppmR1)/(Vin - Vdrop);
  Rc = Rc - ppmRa;
  K = 1000 / (Rc * EC);
   
  ppmK = K;
}
