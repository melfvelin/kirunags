#ifndef __test1__h
#define __test1__h  

#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iomanip>
#include <cmath>
#include <chrono>

#include "TLE.h"
#include "SGP4.h"

void testImport();
long mSecSince1970();
void teme2pef(double *vec, double gsto);
void pef2sez(double *dpVecSez, double *dpVecPef, double dLatGeod, double dLonGeod);
double getGeocentricLatitude(double dLatGeod);
void printDate();
double currentJdut1(std::time_t *t);
void gsCoord(double *pdCoordVec, double dlatGeod, double dLonGeod, double dAltitude);
void getAngles(double *dGsSatVecSez, double& dElev, double& dAz, double& dRange);
double rad2deg(double dRad);

#endif
