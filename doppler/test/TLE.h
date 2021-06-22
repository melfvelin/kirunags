#ifndef __sgp4tleheader__
#define __sgp4tleheader__  

#include <iostream>
#include <ostream>
#include "SGP4.h"
#include "orbitPrediction.h"
#include "SatTrack.h"

#define TRUE 1
#define FALSE 0

class TLE
{
  public:
    elsetrec rec;
    char line1[70];
    char line2[70];
    char intlid[12];
    char objectID[6];
    long epoch;
    double ndot;
    double nddot;
    double bstar;
    int elnum;
    double incDeg;
    double raanDeg;
    double ecc;
    double argpDeg;
    double maDeg;
    double n;
    int revnum;
    int sgp4Error;


  public:

    TLE();

    TLE(char *line1, char *line2);

    void parseLines(char *line1, char *line2);

    void getRVForDate(long millisSince1970, double r[3], double v[3]);

    void getRV(double minutesAfterEpoch, double r[3], double v[3]);

    void setValsToRec(TLE *tle, elsetrec& rec);

};

//void setValsToRec(TLE *tle, elsetrec& rec);

#endif
