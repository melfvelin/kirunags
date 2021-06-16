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
void printDate();
double currentJdut1();

#endif
