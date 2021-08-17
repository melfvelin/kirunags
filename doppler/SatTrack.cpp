#include "test1.h"


/* dopplerShift - takes two slant ranges separated by Dt seconds and returns the doppler shift per 100 MHz
*   inputs: dRange - range to sat at t0, dRangeDt - range to sat at t0 + dt, Dt - delta t in seconds
*   outputs: returns Doppler shift per 100 MHz of carrier frequency
*   author: Martin Elfvelin
*/
double dopplerShift(double dRange, double dRangeDt, double Dt)
{
    double Vrel = (dRangeDt - dRange)/Dt;
    
    #ifdef debug
        std::cout << "Relative velocity: " << std::fixed << std::setprecision(4) << Vrel << std::endl;
        std::cout << "using delta t: " << std::fixed << std::setprecision(2) << Dt << std::endl;
    #endif /* debug */
    
    double dShift = Vrel * 100 / 300;       // 100 MHz / 300 M m/s
    return dShift*1000;
}

/* teme2pef - takes a position vector in TEME and rotates it to PEF by matrix multiplication using GMST 
*   inputs: dpPosVecTeme - pointer to a position vector in TEME
*           gsto - Greenwich mean sidereal time (angle between the Greenwich meridian and the vernal equinox)
*   outputs: dpPosVecPef - PEF coordinates in vector pointed to
*   function calls: cos(), sin() (cmath)
*   author: Martin Elfvelin
*/
void teme2pef(double *pdPosVecPef, double *pdPosVecTeme, double gsto)
{
    double rotMat[3][3];

    rotMat[0][0] = cos(gsto);       rotMat[0][1] = sin(gsto);   rotMat[0][2] = 0;
    rotMat[1][0] = -1 * sin(gsto);  rotMat[1][1] = cos(gsto);   rotMat[1][2] = 0;
    rotMat[2][0] = 0;               rotMat[2][1] = 0;           rotMat[2][2] = 1;

    pdPosVecPef[0] = rotMat[0][0] * pdPosVecTeme[0] + rotMat[0][1] * pdPosVecTeme[1] + rotMat[0][2] * pdPosVecTeme[2];
    pdPosVecPef[1] = rotMat[1][0] * pdPosVecTeme[0] + rotMat[1][1] * pdPosVecTeme[1] + rotMat[1][2] * pdPosVecTeme[2];
    pdPosVecPef[2] = rotMat[2][0] * pdPosVecTeme[0] + rotMat[2][1] * pdPosVecTeme[1] + rotMat[2][2] * pdPosVecTeme[2];
    
    return;
}

/* pef2sez - takes a position vector in PEF and rotates it to SEZ using matrix multiplication with
*   geodetic longitude and latitude
*   inputs: dpVecPef - pointer to a position vector in PEF
*           dLatgeod - geodetic latitude
*           dLonGeod - geodetic longitude
*   outputs: dpVecSez - SEZ coordinates in vector pointed to
*   function calls: cos(), sin() (cmath), getGeocentricLatitude()
*   author: Martin Elfvelin
*/
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

/* getGeocentricLatitude - takes a geodetic latitude and returns the corresponding 
*   geocentric latitude
*   inputs: dLatGeod - geodetic latitude (rad)
*   outputs: returns geocentric latitude (rad)
*   note: could be improved by using atan2()?
*   function calls: atan (cmath)
*   author: Martin Elfvelin
*/
double getGeocentricLatitude(double dLatGeod)
{
    double m_dEarthR = 6378.137;        // Earth radius
    double m_dEarthPolR = 6356.755;     // Earth polar radius

    // atan2(m_dEarthPolR, m_dEarthR) * tan(dLatGeod); is this also good
    return atan((m_dEarthPolR/m_dEarthR) * tan(dLatGeod));
}

/*  currentJdut1 - returns the Julian Day in UT1 based on the input time
*   inputs: pointer to time_t object
*   outputs: returns Jdut1 (number of days + fractional day since 4713 BC)
*   function calls: jday() - (SGP4, Vallado), gmtime <ctime>
*   author: Martin Elfvelin
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

/* gsCoord - takes geodetic lat, long and altitude and computes gs site vector in PEF
*   input: dLatGeod - geodetic latitude, dLonGeod - geodetic longitude,
*                       dAltitude - height over geoid in km
*   outputs: GS site vector in PEF coordinate frame
*   author: Martin Elfvelin
*   function calls: sqrt(), pow(), cos(), sin() - (cmath), getGeocentricLatitude()
*/
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

void getlatlon(double *dpPosVec, double *dpLatLon)
{
    double x = dpPosVec[0];
    double y = dpPosVec[1];
    double z = dpPosVec[2];

    const double a = 6378137;
    const double f = 0.0034;
    const double b = 6356800;
    const double e = sqrt((pow(a,2) - pow(b,2)) / pow(a,2));
    const double e2 = sqrt((pow(a,2) - pow(b,2)) / pow(b,2));

    double m_dLat, m_dLon, m_dHeight, m_dN, m_dTheta, m_dP;

    m_dP = sqrt(pow(x, 2), pow(y, 2));
    m_dTheta = atan2(a*z, b*m_dP);
    m_dLon = atan2(y, x);
    m_dLat = atan2((z+pow(e2, 2)*b*pow(sin(m_dTheta), 3)), (p-pow(e,2)*a*cos(m_dTheta)));
    m_dN = a / sqrt(1 - pow(e, 2)*pow(sin(m_dLat), 2));

    double m = (p / cos(lat));
    m_dHeight = m - N;

    dpLatLon[0] = m_dLat;
    dpLatLon[1] = m_dLon;
    dpLatLon[2] = m_dHeight;

    return;
}

/* getAngles - takes a GS-Satellite SEZ vector and computes elevation, azimuth and range
*   input: dGsSatVecSez - double pointer to GS-satellite vector
*   outputs: Elevation (rad), Azimuth (rad, 0-2pi clockwise from north), range (km)
*   function calls: sqrt(), pow(), asin(), atan() - (cmath)
*   author: Martin Elfvelin
*/
void getAngles(double *dGsSatVecSez, double& dElev, double& dAz, double& dRange)
{
    
    // element 0 = px, 1 = py, 2 = pz where p is the gs-sat vector in SEZ
    double pow1 = pow(dGsSatVecSez[0], 2);
    double pow2 = pow(dGsSatVecSez[1], 2);
    double pow3 = pow(dGsSatVecSez[2], 2);

    dRange = sqrt((pow1 + pow2 + pow3 ));

    // must use atan2 to get -pi to pi range
    dAz = M_PI - atan2(dGsSatVecSez[1], dGsSatVecSez[0]);
    dElev = asin(dGsSatVecSez[2] / dRange);

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

/* passFinder - finds all passes up to 2 days into the future (from time of exec), returns how many passes it found
*   inputs: none
*   outputs: double array with sys times during which elevation will be > 0 deg, one time corresponds to one pass
*   returns: passcount 
*   author: Martin Elfvelin
*   function calls: time() - <ctime>, instantPredict(), 
*/
int passFinder(double *pdPassTimes)
{
    double m_dRange[1440];
    double m_dElev[1440];
    double m_dAz[1440];
    double m_dTimeStamp[1440];
    double *m_pdResults = nullptr;
    m_pdResults = new double[5];
    int passcount = 0;

    // get sys time
    std::time_t timeObject = std::time(0);
    std::time_t tNow = timeObject;

    // This loop calculates state vectors for 2 days forward from tNow with 120 sec step length
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
        if(m_dElev[i] > 0.0 && i > 0 && m_dElev[i-1] <= 0.0)
        {
            pdPassTimes[passcount] = m_dTimeStamp[i];
            passcount++;

            #ifdef debug
                std::cout << "Pass number: " << passcount ;
                std::cout << " found at timestamp: " << std::fixed << std::setprecision(2) << m_dTimeStamp[i] << std::endl;
            #endif
        }
    }

    for(int i = 0; i < passcount; i++)
    {
        #ifdef debug
            std::cout << " Timestamp: " << std::fixed << std::setprecision(2) << pdPassTimes[i] << std::endl;
        #endif /* debug */
    }

    return passcount;
}

/*  getDopplerProfile - takes AOS time and prints range, angles and doppler shift (Hz per 100MHz)
*       for the whole pass.
*   inputs: double dTimeStamp (sys time) has to be AOS time or time during pass (elevation > 0 deg)
*   outputs: none
*   note: not used atm, does not output anything other than terminal output, could be used to
*   print angles and doppler to a file with minor modifications
*   author: Martin Elfvelin
*/
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

/*  printDate - takes an std::time_t object and prints the date as yyy-mm-dd
*   inputs: time_t t (seconds since Jan 1970)
*   outputs: none
*   author: Martin Elfvelin
*/
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

    std::tm* now = std::gmtime(&t);

    year = now->tm_year + 1900;
    mon = now->tm_mon + 1;
    day = now->tm_mday;
    hour = now->tm_hour;
    min = now->tm_min;
    sec = now->tm_sec;

    if(mon < 10 || day < 10)
    {
        if(mon < 10 && day < 10)
        {
            std::cout << (now->tm_year + 1900) << "-0" << (now->tm_mon + 1) << "-0" <<  now->tm_mday << "\n";
        }
        else if(mon < 10)
        {
            std::cout << (now->tm_year + 1900) << "-0" << (now->tm_mon + 1) << "-" <<  now->tm_mday << "\n";
        }
        else if(day < 10)
        {
            std::cout << (now->tm_year + 1900) << "-" << (now->tm_mon + 1) << "-0" <<  now->tm_mday << "\n";
        }
    }
    else
    {
        std::cout << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday << "\n";
    }

    // Using Vallado's jday function from SGP4.c or SGP4.cpp this is not used
    jday(year, mon,  day, hour, min, sec, &j_day, &j_dayfrac);

    #ifdef debug
        std::cout << "j_day: " <<  std::fixed << std::setprecision(2) << j_day << std::endl;   
        std::cout << "j_dayfrac: " <<  std::fixed << std::setprecision(5) << j_dayfrac << std::endl;
        std::cout << "combined jday: " <<  std::fixed << std::setprecision(5) << j_day + j_dayfrac << std::endl;   
    #endif /* debug */

    return;
}

/*  printDateTime - takes an std::time_t object and prints the date and time as yyy-mm-dd-hh-mm-ss
*   inputs: time_t t (seconds since Jan 1970)
*   outputs: none
*   author: Martin Elfvelin
*/
void printDateTime(std::time_t t)
{
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;

    std::tm* now = std::gmtime(&t);
    year = now->tm_year + 1900;
    mon = now->tm_mon + 1;
    day = now->tm_mday;
    hour = now->tm_hour;
    min = now->tm_min;
    sec = now->tm_sec;

    if(mon < 10 || day < 10)
    {
        if(mon < 10 && day < 10)
        {
            std::cout << (now->tm_year + 1900) << "-0" << (now->tm_mon + 1) << "-0" <<  now->tm_mday << "-";
        }
        else if(mon < 10)
        {
            std::cout << (now->tm_year + 1900) << "-0" << (now->tm_mon + 1) << "-" <<  now->tm_mday << "-";
        }
        else if(day < 10)
        {
            std::cout << (now->tm_year + 1900) << "-" << (now->tm_mon + 1) << "-0" <<  now->tm_mday << "-";
        }
    }
    else
    {
        std::cout << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday << "-";
    }

    
    if(hour < 10 || min < 10 || sec < 10)
    {
        if(hour < 10 && min < 10 && sec < 10)
        {
            std::cout << "0" << hour << ":0" << min << ":0" << sec  ;
        }
        else if(hour < 10 && min < 10)
        {
            std::cout << "0" << hour << ":0" << min << ":" << sec  ;
        }
        else if(hour < 10 && sec < 10)
        {
            std::cout << "0" << hour << ":" << min << ":0" << sec  ;
        }
        else if(hour < 10)
        {
            std::cout << "0" << hour << ":" << min << ":" << sec  ;
        }
        else if(min < 10 && sec < 10)
        {
            std::cout << "" << hour << ":0" << min << ":0" << sec  ;
        }
        else if(min < 10)
        {
            std::cout << "" << hour << ":0" << min << ":" << sec  ;
        }
        else if(sec < 10)
        {
            std::cout << "" << hour << ":" << min << ":0" << sec  ;
        }
    }
    else
    {
        std::cout << hour << ":" << min << ":" << sec  ;    
    }
    
    return;
}

/* findAOSLOS - takes a time for when satellite is visible and finds AOS and LOS
*   inputs: dStartTime - sys time when chosen satellite is visible from GS
*   outputs:    dAosTime - sys time when satellite becomes visible
*               dLosTime - sys time when satellite stops being visible
*   returns 0 on success, -1 on failure
*   function calls: instantPredict(), printDateTime() (only in debug mode)
*   author: Martin Elfvelin
*/
int findAOSLOS(double dStartTime, double& dAosTime, double& dLosTime)
{
    // currently only uses elevation angle but can be expanded to include Azimuth and range
    double m_dElev;     
    double m_dRange;    
    double m_dAz;
    double m_dDoppler;
    double m_dTimeStep = 1;
    double m_dTimeStamp;
    double *m_pdResults = nullptr;
    m_pdResults = new double[5];

    m_dTimeStamp = dStartTime;
    // first we get angles for the input time
    m_pdResults = instantPredict(dStartTime);

    // store results
    m_dElev = m_pdResults[0];
    m_dAz = m_pdResults[1];
    m_dRange = m_pdResults[2];
    m_dDoppler = m_pdResults[3];

    // check that satellite is visible at given time
    if(m_dElev < 0.0)
    {
        // return -1 if satellite is not visible
        return -1;
    }

    // then we find AOS time by moving backwards from start time
    while(m_dElev > 0.0)
    {
        m_dTimeStamp = m_dTimeStamp - m_dTimeStep;
        m_pdResults = instantPredict(m_dTimeStamp);
        m_dElev = m_pdResults[0];
    }

    m_dTimeStamp = m_dTimeStamp + 1;
    m_pdResults = instantPredict(m_dTimeStamp);

    // store results
    m_dElev = m_pdResults[0];
    m_dAz = m_pdResults[1];
    m_dRange = m_pdResults[2];
    m_dDoppler = m_pdResults[3];
    m_dTimeStamp = m_pdResults[4];

    #ifdef debug
        std::cout << "[findAOSLOS] Elevation will be: " << std::fixed << std::setprecision(3) << m_dElev << std::endl;
        std::cout << "at ";
        printDateTime(m_dTimeStamp);
    #endif /* debug */
    
    // store output and reset time stamp to start time
    dAosTime = m_dTimeStamp;
    m_dTimeStamp = dStartTime;
    // now we find LOS time 
    m_pdResults = instantPredict(dStartTime);

    // store results
    m_dElev = m_pdResults[0];
    m_dAz = m_pdResults[1];
    m_dRange = m_pdResults[2];
    m_dDoppler = m_pdResults[3];

    // now we find LOS by moving forwards from start time
    while(m_dElev > 0.0)
    {
        m_dTimeStamp = m_dTimeStamp + m_dTimeStep;
        m_pdResults = instantPredict(m_dTimeStamp);
        m_dElev = m_pdResults[0];
    }

    #ifdef debug
        std::cout << "[findAOSLOS] Elevation will be: " << std::fixed << std::setprecision(3) << m_dElev << std::endl;
        std::cout << "at ";
        printDateTime(m_dTimeStamp);
    #endif /* debug */
    
    dLosTime = m_dTimeStamp;
    delete[] m_pdResults;
    return 0;
} 
/* printLivePass - takes a time_t object with AOS time and prints out antenna angles live
*   inputs: time_t AOS specifying an AOS time as given by findAOSLOS
*   outputs: none
*   returns: -1 if pass is in the past, 0 after pass is finished
*   note: this function cannot print angles for a pass that has already started, although it should
*   be an easy fix
*   author: Martin Elfvelin
*/
int printLivePass(std::time_t AOS)
{
    double m_dElev;     
    double m_dRange;    
    double m_dAz;
    double m_dDoppler;
    double *m_pdResults = nullptr;
    m_pdResults = new double[5];
    std::time_t tNow = time(0);
    uint32_t m_nMicrosec = 1000000;

    if(tNow > AOS)
    {
        std::cout << "Pass is in the past!" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Waiting for pass to start..." << std::endl;
        while(tNow < AOS)
        {
            // waiting for pass to start
            tNow = time(0);
        }
        std::cout << "Pass starting..." << std::endl;
        m_pdResults = instantPredict(tNow);
        m_dElev = m_pdResults[0];
        
        while(m_dElev > 0.0)
        {
            // get angles and print here
            // use a delay function
            tNow = time(0);
            m_pdResults = instantPredict(tNow);
            m_dElev = m_pdResults[0];
            m_dAz = m_pdResults[1];
            m_dRange = m_pdResults[2];
            m_dDoppler = m_pdResults[3];
            std::cout << "El: " << std::fixed << std::setprecision(3) << m_dElev;
            std::cout << "  Az: " << m_dAz << "  Range: " << m_dRange;
            std::cout << "  Doppler: " << m_dDoppler << std::endl;
            usleep(m_nMicrosec * 1);       // sleep for 1000 ms
        }
    }

    return 0;
}

/* main() - contains examples on how to use tracking functions
*
*
*/
int main(void)
{
    double Angles[5];
    double *doubPtr;
    double *m_pdPassTimes = nullptr;
    m_pdPassTimes = new double[100];
    int m_nPass;
    int m_nPasscount;
    double m_dAosTime;
    double m_dLosTime;

    std::time_t timeArg = time(0);
    doubPtr = instantPredict(timeArg);

    /* // simple instant predict 
    for(int i = 0; i < 5; i++)
    {
        Angles[i] = doubPtr[i];
        std::cout << "Angles: " << std::fixed << std::setprecision(4) << *(doubPtr + i) << std::endl;
    }
    */ 

    // passFinder finds passes for the next 2 days
    if((m_nPasscount = passFinder(m_pdPassTimes)) == 0)
    {
        std::cout << "Pass did not find any passes" << std::endl;   
    }
    // passes found, print out AOS and LOS times
    else
    {   
        // implementing a pass ID might be helpful
        std::cout << "Pass finder found " << m_nPasscount << " passes" <<  std::endl;      
        std::cout << "with the following AOS/LOS times: " << std::endl;
        // loop over all found passes and print out AOS/LOS times              
        for(int i = 0; i < m_nPasscount; i++)
        {
            if (findAOSLOS(m_pdPassTimes[i], m_dAosTime, m_dLosTime) == -1)
            {
                std::cout << "Error in findAOSLOS" << std::endl;   
            }
            else
            {
                std::cout << "Pass number: " << i;
                std::cout << " AOS: ";
                printDateTime(m_dAosTime);
                std::cout << " LOS: ";
                printDateTime(m_dLosTime);
                std::cout << std::endl;
            }
        }
    }

    // from passes found we can select one to live track
    std::cout << m_nPasscount << " passes were found, pls select one to get angles for: " << std::endl;
    std::cin >> m_nPass;
    
    // this function only prints out all angles to terminal
    //getDopplerProfile(m_pdPassTimes[m_nPass]);

    //printDateTime(testtime);
    //printDateTime(m_pdPassTimes[0]);

    // Find AOS and LOS times for selected pass use return code to check for errors
    if (findAOSLOS(m_pdPassTimes[m_nPass], m_dAosTime, m_dLosTime) == -1)
    {
        std::cout << "Error in findAOSLOS" << std::endl;    
    }

    std::cout << "For the chosen pass:" << std::endl << "AOS: ";
    printDateTime(m_dAosTime);
    std::cout << std::endl;
    std::cout << "LOS: ";
    printDateTime(m_dLosTime);
    std::cout << std::endl;
    
    printLivePass(m_dAosTime);

    delete[] m_pdPassTimes;
    delete doubPtr;
    return 0;
}