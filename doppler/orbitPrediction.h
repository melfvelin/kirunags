// orbitPrediction.h

#ifndef __ORBITPREDICTION__H
#define __ORBITPREDICTION__H  

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
#include "SatTrack.h"

double *instantPredict(std::time_t timeObject, char *line1, char *line2);

#endif /* __ORBITPREDICTION__H  */

#ifndef debug
//#define debug
#endif /* debug */