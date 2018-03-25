#ifndef PPM_H
#define PPM_H

extern double lastEC;
extern long int lastPPM;
extern double ppmK;

void readPPMValues(double temp);

void calibratePPM(double temp);

#endif
