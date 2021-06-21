#ifndef __SATTRACK__H
#define __SATTRACK__H  

#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <unistd.h>

#include "TLE.h"
#include "SGP4.h"
#include "orbitPrediction.h"

void testImport();
double dopplerShift(double dRange, double dRangeDt, double Dt);
long mSecSince1970();
void teme2pef(double *pdPosVecPef, double *pdPosVecTeme, double gsto);
void pef2sez(double dpVecSez[], double *dpVecPef, double dLatGeod, double dLonGeod);
double getGeocentricLatitude(double dLatGeod);
void printDate(std::time_t t);
void printDateTime(std::time_t t);
double currentJdut1(std::time_t *t);
void gsCoord(double *pdCoordVec, double dlatGeod, double dLonGeod, double dAltitude);
void getAngles(double *dGsSatVecSez, double& dElev, double& dAz, double& dRange);
double rad2deg(double dRad);
double deg2Rad(double dDeg);
int passFinder(double *m_pdPassTimes);
int findAOSLOS(double dStartTime, double& dAosTime, double& dLosTime);
void getDopplerProfile(double dTimeStamp);

#endif
