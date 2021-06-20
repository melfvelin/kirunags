#include "test1.h"

typedef struct VERIN {
    char line1[70];
    char line2[70];
    double startmin;
    double stepmin;
    double stopmin;
} VERIN;

/**
 * returns the count of verins read and sets the point to an array created with malloc.
 */
int readVERINs(VERIN **listptr)
{
    char line[256];
    char *str = NULL;
    FILE *in_file = NULL;
    VERIN *verins = NULL;
    int cnt = 0;


    in_file = fopen("../../data/SGP4-VER.TLE","r");
    
    while(fgets(line,255,in_file) != NULL)
    {
        if(line[0]=='1')cnt++;
    }

    if(in_file)
    {
        fclose(in_file);
    }

    verins = (VERIN*)malloc(cnt*sizeof(VERIN));
    *listptr = verins;

    cnt = 0;
    in_file = fopen("../../data/SGP4-VER.TLE","r");
    
    while(fgets(line,255,in_file) != NULL)
    {
        if(line[0]=='1')
        {
            strncpy(verins[cnt].line1,line,69);
            verins[cnt].line1[69]=0;
            fgets(line,255,in_file);
            strncpy(verins[cnt].line2,line,69);
            verins[cnt].line2[69]=0;
            str = &line[70];
            sscanf(str,"%lf %lf %lf",&verins[cnt].startmin,&verins[cnt].stopmin,&verins[cnt].stepmin);
            cnt++;
        }
    }

    if(in_file)
    {
        fclose(in_file);
    }

    return cnt;
}

/**
 * 2-norm distance for two three vectors
 */
double dist(double *v1, double *v2)
{
    double dist = 0;
    double tmp = 0;

    // just unroll the loop
    tmp = v1[0]-v2[0];
    dist += tmp*tmp;
    tmp = v1[1]-v2[1];
    dist += tmp*tmp;
    tmp = v1[2]-v2[2];
    dist += tmp*tmp;

    return sqrt(dist);
}

void runVER(VERIN *verins, int cnt)
{
    FILE *in_file = NULL;
    TLE tle;
    double r[3];
    double v[3];
    double rv[3];
    double vv[3];
    double mins = 0;
    char line[256];
    int i=-1;
    char *ind = NULL;
    double rdist = 0;
    double vdist = 0;
    double rerr = 0;
    double verr = 0;
    int cnt2 = 0;

    in_file = fopen("../../data/tcppver.out","r");

    while(fgets(line,255,in_file) != NULL && i <cnt)
    {
        ind = strstr(line,"xx");
        if(ind != NULL)
        {
            i++;
            // parseLines is called with two arguments (char pointers)
            tle.parseLines(verins[i].line1,verins[i].line2);
        } 
        else
        {
            sscanf(line,"%lf %lf %lf %lf %lf %lf %lf\n",&mins,rv,&rv[1],&rv[2],vv,&vv[1],&vv[2]);
            tle.getRV(mins,r,v);
            rdist = dist(r,rv);
            vdist = dist(v,vv);
            rerr += rdist;
            verr += vdist;
            cnt2++;
            if(rdist > 1e-7 || vdist > 1e-8)
            {
                printf("%s %lf %e %e\n",tle.objectID,mins,rdist,vdist);
            }
        }
    }

    rerr = rerr/cnt2;
    verr = verr/cnt2;

    printf("Typical edits r=%e mm, v=%e mm/s\n",1e6*rerr,1e6*verr);

    if(in_file)
    {
        fclose(in_file);
    }
}

void testImport()
{
    char satID[40];
    char lineOne[70];
    char lineTwo[70];
    FILE *in_file = NULL;
    char m_cOutputLine[256];
    int m_nLineIndex;
    TLE tle;
    
    // double gsto = gstime_SPG4(jdut1);


    // std::cout << "############## testImport() ###########" << std::endl;
    in_file = fopen("tles/aalto1.TLE","r");

    if(in_file != NULL)
    {
        std::cout << "File opened successfully" << std::endl;    
    }

    if ((fgets(satID, 39, in_file)) != NULL )
    {
        std::cout << "Satellite ID read successfully" << std::endl;       
    }
    
    if ((fgets(lineOne, 255, in_file)) != NULL )
    {
        std::cout << "First line read successfully" << std::endl;       
    }

    if ((fgets(lineTwo, 255, in_file)) != NULL )
    {
        std::cout << "Second line read successfully" << std::endl;       
    }

    if(in_file)
    {
        fclose(in_file);
    }

    std::cout << "Sat ID: " << satID << std::endl;
    std::cout << "First line: " << lineOne << std::endl;
    std::cout << "Second line: " << lineTwo << std::endl;

    tle.parseLines(lineOne, lineTwo);
    std::cout << "Return from parseLines" << std::endl;
    
    // fix the reference to satrec
    //double jdut1 = (tle.rec.jdsatepoch + tle.rec.jdsatepochF) - 2433281.5; // don't need the JD of satellite epoch in this file
    long time = mSecSince1970();
    long m_nPassTimes[1440];
    double m_dJDPassTimes[1440];
    double m_dPosVec[1440][3];
    double m_dVelVec[1440][3];
    double *m_pnPos;
    double *m_pnVel;
    double m_dJdayTLE = tle.rec.jdsatepoch + tle.rec.jdsatepochF;
    double m_dJdayNow;

    // The epoch used in SGP4 is days from jan 0, 1950. 0 hr
    
    std::time_t timeObject = std::time(0);
    m_dJdayNow = currentJdut1(&timeObject);
    double gsto = gstime(m_dJdayNow);

    #ifdef debug
    std::cout << "GMST (deg) = " << std::fixed << std::setprecision(5) << rad2deg(gsto) << std::endl;
    std::cout << "Current JD = " << std::fixed << std::setprecision(8) << m_dJdayNow << std::endl;
    #endif /* debug */

    std::time_t tNow = std::time(0);
    // This loop calculates state vectors for 2 days forward from NOW with 120 sec step
    for(int i = 0; i < 1440; i++)
    {
        // insert jdates here?
        //m_nPassTimes[i] = time + (2 * 60 * 1000 * i);
        //timeObject += 2 * 60;
        timeObject = tNow + 2 * 60 * i;                   // will start at Tnow + 2 min and end at Tnow + 2 days
        m_nPassTimes[i] = timeObject * 1000;
        m_dJDPassTimes[i] = currentJdut1(&timeObject);
        m_pnPos = &m_dPosVec[i][0];
        m_pnVel = &m_dVelVec[i][0];
        // skip pointers? arrays can be used directly

        tle.TLE::getRVForDate(m_nPassTimes[i], m_pnPos, m_pnVel);

        if(i == 0)
        {
            std::cout << "ECI P0: " << std::fixed << std::setprecision(5) << m_dPosVec[i][0] << std::endl;
            std::cout << "ECI P1: " << std::fixed << std::setprecision(5) << m_dPosVec[i][1] << std::endl;
            std::cout << "ECI P2: " << std::fixed << std::setprecision(5) << m_dPosVec[i][2] << std::endl;
        }
        gsto = gstime(m_dJDPassTimes[i]);
        //teme2pef(m_pnPos, gsto);

        if(i == 0)
        {
            std::cout << "ECF P0: " << std::fixed << std::setprecision(5) << m_dPosVec[i][0] << std::endl;
            std::cout << "ECF P1: " << std::fixed << std::setprecision(5) << m_dPosVec[i][1] << std::endl;
            std::cout << "ECF P2: " << std::fixed << std::setprecision(5) << m_dPosVec[i][2] << std::endl;
        }
    }

    #ifdef debug
    std::cout << "Start JD = " << std::fixed << std::setprecision(5) << m_dJDPassTimes[0] << std::endl;
    std::cout << "End JD = " << std::fixed << std::setprecision(5) << m_dJDPassTimes[1439] << std::endl;
    #endif /* debug */

    // rotate Position vector from TEME (SGP4 output) to PEF (ECF)
    double Pvec[3] = {0};
    double Vvec[3] = {0};
    double PvecDt[3] = {0};
    double VvecDt[3] = {0};
    double m_dPosVecPef[3] = {0};
    double m_dPosVecPefDt[3] = {0};
    double Dt = 1;
    // using the same gmst for delta t = 10 sec
    tle.TLE::getRVForDate(timeObject*1000, Pvec, Vvec);     // may need to send in a milisecsince1970 long
    tle.TLE::getRVForDate((timeObject*1000 + 1000 * Dt), PvecDt, VvecDt);
    teme2pef(m_dPosVecPef, Pvec, gsto);
    teme2pef(m_dPosVecPefDt, PvecDt, gsto);

    /* FIND PASSES:
    *   The satellite r vector needs to be converted to SEZ and added with the r_site vector (ground station location) to find
    *   passes (Elevation > 0 deg). Once passes are found we can compute the Doppler profile for the pass
    *   r_ECI = r_siteECI + p_ECI where p_ECI is the SEZ->ECI rotation of the ground station Az, El, Range vector  
    */  


    /*  DOPPLER SHIFT:
    * Now we have satellite r and v vectors in Earth-Centered Inertial (ECI) and need to either convert our ground station r and v vectors
    *   from Topocentric coordinate system (SEZ) to ECI or convert satellite r and v to SEZ to be able to calculate the relative velocity
    *   once the relative velocity is known it is simple to compute the Doppler shift based on the velocity and carrier frequency
    *
    */

    /*
    for(int i = 0; i < 3; i++)
    {
        std::cout << "r[" <<  i << "] = " << std::fixed << std::setprecision(2) << m_dPosVec[1][i];
        std::cout << " v[" << i << "] = " <<  std::fixed << std::setprecision(2) << m_dVelVec[1][i] << std::endl;        
    }


    for(int i = 0; i < 3; i++)
    {
        std::cout << "r[" <<  i << "] = " << std::fixed << std::setprecision(2) << m_dPosVec[1300][i];
        std::cout << " v[" << i << "] = " <<  std::fixed << std::setprecision(2) << m_dVelVec[1300][i] << std::endl;        
    }
    */

    // Computation of ground station position vector in PEF
    double m_dGsVecPef[3];
    double m_dLatGeod = deg2Rad(67.8404);
    double m_dLonGeod = deg2Rad(20.4105);
    double m_dAltitude = 0.4;       // km?
    
    gsCoord(m_dGsVecPef, m_dLatGeod, m_dLonGeod, m_dAltitude);
    
    for(int i = 0; i < 3; i++)
    {
        std::cout << " P_pef[" <<  i << "] = " << std::fixed << std::setprecision(4) << m_dGsVecPef[i];
     
    }
    std::cout << " " << std::endl;

    
    // Computation of terminal - satellite vector in PEF (ECF) coordinates
    double m_dGsSatVecPef[3] = {0};
    double m_dGsSatVecPefDt[3] = {0};

    for(int i = 0; i < 3; i ++)
    {
        m_dGsSatVecPef[i] = m_dPosVecPef[i] - m_dGsVecPef[i];
        m_dGsSatVecPefDt[i] = m_dPosVecPefDt[i] - m_dGsVecPef[i];        
    }
    
    double m_dGsSatVecSez[3];
    double m_dGsSatVecSezDt[3];

    // Transformation of ground station - satellite vector to topocentric (SEZ) coordinates
    pef2sez(m_dGsSatVecSez, m_dGsSatVecPef, m_dLatGeod, m_dLonGeod);
    pef2sez(m_dGsSatVecSezDt, m_dGsSatVecPefDt, m_dLatGeod, m_dLonGeod);

    for(int i = 0; i < 3; i++)
    {
        std::cout << " Psez[" <<  i << "] = " << std::fixed << std::setprecision(4) << m_dGsSatVecSez[i] << std::endl;
     
    }

    double m_dRange;
    double m_dElev;
    double m_dAz;

    double m_dRangeDt;
    double m_dElevDt;
    double m_dAzDt;

    // Get angles and distance for gs-sat vector
    getAngles(m_dGsSatVecSez, m_dElev, m_dAz, m_dRange);
    getAngles(m_dGsSatVecSezDt, m_dElevDt, m_dAzDt, m_dRangeDt);

    std::cout << "Elevation: " << std::fixed << std::setprecision(4) << rad2deg(m_dElev) << std::endl;
    std::cout << "Azimuth: " << std::fixed << std::setprecision(4) << rad2deg(m_dAz) << std::endl;
    std::cout << "Range: " << std::fixed << std::setprecision(4) << m_dRange << std::endl;


    std::cout << "Doppler shift (100MHz): " << std::fixed << std::setprecision(3) << dopplerShift(m_dRange, m_dRangeDt, Dt) << std::endl;
    return;
}

double dopplerShift(double dRange, double dRangeDt, double Dt)
{
    double Vrel = (dRangeDt - dRange)/Dt;
    
    #ifdef debug
        std::cout << "Relative velocity: " << std::fixed << std::setprecision(4) << Vrel << std::endl;
        std::cout << "using delta t: " << std::fixed << std::setprecision(2) << Dt << std::endl;
    #endif /* debug */
    
    double dShift = Vrel * 100 / 300;
    return dShift*1000;
}

// Take the current time and return epoch? then make it a vector of epochs
long mSecSince1970()
{
    time_t m_secSince1970;
    long m_nMiliSec;
    m_secSince1970 = time(NULL);        // returns seconds since gmt 00:00 1970
    
    m_nMiliSec = (long)m_secSince1970;
    m_nMiliSec = m_nMiliSec*1000;

    return m_nMiliSec;
}

void teme2pef(double *pdPosVecPef, double *pdPosVecTeme, double gsto)
{
    double rotMat[3][3];

    // Need to get Jdate

    rotMat[0][0] = cos(gsto);       rotMat[0][1] = sin(gsto);   rotMat[0][2] = 0;
    rotMat[1][0] = -1 * sin(gsto);  rotMat[1][1] = cos(gsto);   rotMat[1][2] = 0;
    rotMat[2][0] = 0;               rotMat[2][1] = 0;           rotMat[2][2] = 1;

    pdPosVecPef[0] = rotMat[0][0] * pdPosVecTeme[0] + rotMat[0][1] * pdPosVecTeme[1] + rotMat[0][2] * pdPosVecTeme[2];
    pdPosVecPef[1] = rotMat[1][0] * pdPosVecTeme[0] + rotMat[1][1] * pdPosVecTeme[1] + rotMat[1][2] * pdPosVecTeme[2];
    pdPosVecPef[2] = rotMat[2][0] * pdPosVecTeme[0] + rotMat[2][1] * pdPosVecTeme[1] + rotMat[2][2] * pdPosVecTeme[2];
    

    return;
}

void pef2sez(double *dpVecSez, double *dpVecPef, double dLatGeod, double dLonGeod)
{
    double rotMat[3][3];
    double m_dLatGeoc = getGeocentricLatitude(dLatGeod);

    rotMat[0][0] = sin(m_dLatGeoc) * cos(dLonGeod);     rotMat[0][1] = sin(m_dLatGeoc) * sin(dLonGeod);     rotMat[0][2] = -1 * cos(m_dLatGeoc);
    rotMat[1][0] = -1 * sin(dLonGeod);                  rotMat[1][1] = cos(dLonGeod);                       rotMat[1][2] = 0;
    rotMat[2][0] = cos(m_dLatGeoc) * cos(dLonGeod);     rotMat[2][1] = cos(m_dLatGeoc) * sin(dLonGeod);     rotMat[2][2] = sin(m_dLatGeoc);

    dpVecSez[0] = rotMat[0][0] * dpVecPef[0] + rotMat[0][1] * dpVecPef[1] + rotMat[0][2] * dpVecPef[2];
    dpVecSez[1] = rotMat[1][0] * dpVecPef[0] + rotMat[1][1] * dpVecPef[1] + rotMat[1][2] * dpVecPef[2];
    dpVecSez[2] = rotMat[2][0] * dpVecPef[0] + rotMat[2][1] * dpVecPef[1] + rotMat[2][2] * dpVecPef[2];
    

    return;
}

double getGeocentricLatitude(double dLatGeod)
{
    double m_dEarthR = 6378.137;        // Earth radius
    double m_dEarthPolR = 6356.755;     // Earth polar radius

    return atan((m_dEarthPolR/m_dEarthR) * tan(dLatGeod));
}

/*  currentJdut1 - returns the Julian Day in UT1 (i think) based on the current sys time (UTC)
*
*
*/
double currentJdut1(std::time_t *t)
{
    int m_nYear;
    int m_nMon;
    int m_nDay;
    int m_nHour;
    int m_nMin;
    double m_dSec;
    double m_dJday;
    double m_dJdayFrac;
    
    std::tm* now = std::gmtime(t);
    
    m_nYear = now->tm_year + 1900;
    m_nMon = now->tm_mon + 1;
    m_nDay = now->tm_mday;
    m_nHour = now->tm_hour;
    m_nMin = now->tm_min;
    m_dSec = now->tm_sec;

    // Using Vallado's jday function from SGP4.c or SGP4.cpp
    jday(m_nYear, m_nMon, m_nDay, m_nHour, m_nMin, m_dSec, &m_dJday, &m_dJdayFrac);

    return m_dJday + m_dJdayFrac;
}

void gsCoord(double *pdCoordVec, double dLatGeod, double dLonGeod, double dAltitude)
{
    double m_dEarthR = 6378.137;        // Earth radius
    double m_dEarthPolR = 6356.755;     // Earth polar radius
    double m_dEarthGSR;                 // Earth radius at ground station
    double m_dTemp;                     // Temp variable for Pythagoras
    double m_dLatGeoc = getGeocentricLatitude(dLatGeod);
    
    #ifdef debug
        std::cout << "Geocentric latitude = " << std::fixed << std::setprecision(3) << rad2deg(m_dLatGeoc) << std::endl; 
    #endif /* debug */
    

    m_dTemp = sqrt((pow(m_dEarthR * sin(m_dLatGeoc), 2)) + (pow(m_dEarthPolR * cos(m_dLatGeoc), 2)) );
    m_dEarthGSR = dAltitude + m_dEarthR * m_dEarthPolR / m_dTemp;

    pdCoordVec[0] = m_dEarthGSR * cos(dLonGeod) * cos(m_dLatGeoc);
    pdCoordVec[1] = m_dEarthGSR * sin(dLonGeod) * cos(m_dLatGeoc);
    pdCoordVec[2] = m_dEarthGSR * sin(m_dLatGeoc);

    return;
}

void getAngles(double *dGsSatVecSez, double& dElev, double& dAz, double& dRange)
{
    
    // check correct angle handling (rad vs deg)
    // element 0 = px, 1 = py, 2 = pz where p is the gs-sat vector in SEZ
    double pow1 = pow(dGsSatVecSez[0], 2);
    double pow2 = pow(dGsSatVecSez[1], 2);
    double pow3 = pow(dGsSatVecSez[2], 2);

    dRange = sqrt((pow1 + pow2 + pow3 ));

    // dRange = sqrt(pow(dGsSatVecSez[0], 2) + pow(dGsSatVecSez[1], 2) + pow(dGsSatVecSez[2], 2));
    dAz = pi - atan(dGsSatVecSez[1] / dGsSatVecSez[0]); // skip the pi minus? Check Jonny  formulas
    dElev = asin(dGsSatVecSez[2] / dRange);

    // std::cout << "Annoying debug Elevation: " << std::fixed << std::setprecision(4) << dRange << std::endl;
    // std::cout << "Annoying debug Azimuth: " << std::fixed << std::setprecision(4) << dAz << std::endl;

    return;
}

double rad2deg(double dRad)
{
    
    return dRad * 180 / M_PI;
}

double deg2Rad(double dDeg)
{
    return dDeg * M_PI / 180;
}

// Finds passes up to 2 days into future, ignore Doppler
int passFinder(double *m_pdPassTimes)
{
    
    //long m_nPassTimes[1440];
    double m_dRange[1440];
    double m_dElev[1440];
    double m_dAz[1440];
    double m_dTimeStamp[1440];
    // double m_dPassTimes[100];
    double *m_pdResults = nullptr;
    m_pdResults = new double[5];
    //double *m_pdPassTimes = nullptr;
    // m_pdPassTimes = new double[100];    // move to main function?
    // verify correct reception of angles
    int elevcount = 0;
    int passcount = 0;

    // get sys time
    std::time_t timeObject = std::time(0);
    std::time_t tNow = timeObject;

    // This loop calculates state vectors for 2 days forward from NOW with 120 sec step
    for(int i = 0; i < 1440; i++)
    {
        // include time stamp in return values?
        timeObject = tNow + 2 * 60 * i;                   // will start at Tnow and end at Tnow + 2 days
        //m_nPassTimes[i] = timeObject * 1000;
        m_pdResults = instantPredict(timeObject);
        m_dElev[i] = m_pdResults[0];
        m_dAz[i] = m_pdResults[1];
        m_dRange[i] = m_pdResults[2];
        m_dTimeStamp[i] = m_pdResults[4];
    }

    // better to put inside above loop? or separate function?
    for(int i = 0; i < 1440; i++)
    {
        if(m_dElev[i] > 0.0)
        {
            elevcount++;

            if(i > 0)       // Could be two conditions in one if
            {
                if(m_dElev[i-1] <= 0.0)     // If previous elevation was <= 0
                {
                    m_pdPassTimes[passcount] = m_dTimeStamp[i];
                    passcount++;

                    #ifdef debug
                        std::cout << "Pass number: " << passcount ;
                        std::cout << " found at timestamp: " << std::fixed << std::setprecision(2) << m_dTimeStamp[i] << std::endl;
                    #endif /* debug */
                    
                    //m_dPassTimes[passcount] = m_dTimeStamp[i];

                }
                else
                {
                    
                }
            }
        }
    }

    for(int i = 0; i < passcount; i++)
    {
        #ifdef debug
            std::cout << " Timestamp: " << std::fixed << std::setprecision(2) << m_pdPassTimes[i] << std::endl;
        #endif /* debug */
    }

    if(passcount > 0)
    {
        return passcount;
    }

    return 0;
} /* passFinder() */

// destroy this
void testArr(double arr[])
{
    arr[0] = 1.5;
    arr[1] = 2.0;
    arr[2] = 3.5;

    return;
}

void getDopplerProfile(double dTimeStamp)
{
    double *m_pdPassTimes = nullptr;
    m_pdPassTimes = new double[1800];
    double m_dRange[1800];
    double m_dElev[1800];
    double m_dAz[1800];
    double m_dDoppler[1800];
    double m_dTimeStamp[1800];
    double *m_pdResults = nullptr;
    m_pdResults = new double[5];
    // Implement AOS and LOS variables

    m_pdPassTimes[0] = dTimeStamp - 60 * 10;        // set start time to 10 minutes before AOS
    std::time_t timeArg = time(0);                  // init a time_t object, stored time will be changed

    for(int i = 0; i < 1800; i++)
    {
        m_pdPassTimes[i] = m_pdPassTimes[0] + i;
    }

    std::cout << "El:   Az:   Range:   Doppler:   " << std::endl;
    // Print date here? Use AOS time?
    // get angles for pass
    for(int i = 0; i < 1000; i++)
    {      
        timeArg = m_pdPassTimes[i];
        m_pdResults = instantPredict(timeArg);      // get angles for each second of the pass, store in doubles
        m_dElev[i] = m_pdResults[0];
        m_dAz[i] = m_pdResults[1];
        m_dRange[i] = m_pdResults[2];
        m_dDoppler[i] = m_pdResults[3];
        m_dTimeStamp[i] = m_pdResults[4];
    
        std::cout << std::fixed << std::setprecision(2) << m_dElev[i] << "  " << m_dAz[i] << "   " << m_dRange[i] <<  "   "
        << m_dDoppler[i] << std::endl;
    }


    return;
}

void printDate(std::time_t t)
{
    int year;
    int mon;
    int day;
    int hour;
    int min;
    double sec;
    double j_day;
    double j_dayfrac;
    double *jd = &j_day;;
    double *jdfrac = &j_dayfrac;


    // std::time_t t = std::time(0);   // get time now
    std::tm* now = std::gmtime(&t);
    std::cout << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday << "\n";

    year = now->tm_year + 1900;
    mon = now->tm_mon + 1;
    day = now->tm_mday;
    hour = now->tm_hour;
    min = now->tm_min;
    sec = now->tm_sec;

    std::cout << "YYYY-MM-DD: " << year << mon <<  day << std::endl;
    std::cout << "HH-MM-SS: " << hour << min <<  sec << std::endl;
    // Using Vallado's jday function from SGP4.c or SGP4.cpp
    jday(year, mon,  day, hour, min, sec, &j_day, &j_dayfrac);

    std::cout << "j_day: " <<  std::fixed << std::setprecision(2) << j_day << std::endl;   
    std::cout << "j_dayfrac: " <<  std::fixed << std::setprecision(5) << j_dayfrac << std::endl;
    std::cout << "combined jday: " <<  std::fixed << std::setprecision(5) << j_day + j_dayfrac << std::endl;   

    return;
}

int main(void)
{

    
    //testImport();
    double Angles[5];
    double *doubPtr;
    double *m_pdPassTimes = nullptr;
    m_pdPassTimes = new double[100];
    int m_nPass;

    int m_nPasscount;
    std::time_t timeArg = time(0);
    doubPtr = instantPredict(timeArg);

    for(int i = 0; i < 5; i++)
    {
        Angles[i] = doubPtr[i];
        std::cout << "Angles: " << std::fixed << std::setprecision(4) << *(doubPtr + i) << std::endl;
    }
    
    if((m_nPasscount = passFinder(m_pdPassTimes)) == 0)
    {
        std::cout << "Pass did not find any passes" << std::endl;   
    }
    else
    {
        std::cout << "Pass finder found " << m_nPasscount << " passes" <<  std::endl;      
    }

    for(int i = 0; i < m_nPasscount; i++)
    {
        std::cout << " (main) Timestamp: " << std::fixed << std::setprecision(2) << m_pdPassTimes[i] << std::endl;
    }

    std::cout << m_nPasscount << " passes were found, pls select one to get angles for: " << std::endl;
    std::cin >> m_nPass;
    // this function modifies pass times? Yes results are stored here
    getDopplerProfile(m_pdPassTimes[m_nPass]);

    delete m_pdPassTimes;
    
    return 0;
}