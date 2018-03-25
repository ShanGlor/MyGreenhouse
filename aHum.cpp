#include "aHum.h"
#include "Settings.h"
#include <Sodaq_SHT2x.h>
#include <Arduino.h>

double lastAHumTemp = 0;
int lastAHumHum = 0;
double lastAHumDew = 0;

void readAHumValues(void){
//  double temp = SHT2x.GetTemperature();
//  double hum = SHT2x.GetHumidity();
//  double dew = SHT2x.GetDewPoint();
  // Check if the difference between previous reading and this one is enough to warrant an immediate report

    lastAHumHum = random(60,99);
    lastAHumTemp = (double)random(240,300)/10;
//  lastAHumTemp = temp;
//  lastAHumHum = hum;
//  lastAHumDew = dew;
}
