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


    std::cout << "############## testImport() ###########" << std::endl;
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
    double jdut1 = (tle.rec.jdsatepoch + tle.rec.jdsatepochF) - 2433281.5;
    long time = mSecSince1970();
    long m_nPassTimes[1440];
    double m_dJDPassTimes[1440];
    double m_dPosVec[1440][3];
    double m_dVelVec[1440][3];
    double *m_pnPos;
    double *m_pnVel;
    double m_dJdayTLE = tle.rec.jdsatepoch + tle.rec.jdsatepochF;
    double m_dJdayNow;
    double testTime = difftime(time, tle.epoch);

    // The epoch used in SGP4 is days from jan 0, 1950. 0 hr
    double diff = (double)time - tle.epoch;
    diff/=60000.0;
    std::cout << "Minutes since epoch: "<< std::fixed << std::setprecision(3) << diff << std::endl;

    // make a time vector with 2 minute spacing and get RV vectors for each time
    
    std::time_t timeObject = std::time(0);
    m_dJdayNow = currentJdut1(&timeObject);
    //m_dJdayNow += 86400/12;
    double gsto = gstime(m_dJdayNow);
    std::cout << "GMST (deg) = " << std::fixed << std::setprecision(5) << gsto*360/twopi << std::endl;
    std::cout << "Current JD = " << std::fixed << std::setprecision(5) << m_dJdayNow << std::endl;
    // then add rotation of today? or is it already included?

    // timeObject += 86400;
    // m_dJdayNow = currentJdut1(&timeObject);
    // std::cout << "Added JD = " << std::fixed << std::setprecision(5) << m_dJdayNow << std::endl;

    for(int i = 0; i < 1440; i++)
    {
        // insert jdates here?
        //m_nPassTimes[i] = time + (2 * 60 * 1000 * i);
        timeObject += 2 * 60;                   // will start at Tnow + 2 min and end at Tnow + 2 days
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
        teme2pef(m_pnPos, gsto);

        if(i == 0)
        {
            std::cout << "ECF P0: " << std::fixed << std::setprecision(5) << m_dPosVec[i][0] << std::endl;
            std::cout << "ECF P1: " << std::fixed << std::setprecision(5) << m_dPosVec[i][1] << std::endl;
            std::cout << "ECF P2: " << std::fixed << std::setprecision(5) << m_dPosVec[i][2] << std::endl;
        }
    }

    std::cout << "Start JD = " << std::fixed << std::setprecision(5) << m_dJDPassTimes[0] << std::endl;
    std::cout << "End JD = " << std::fixed << std::setprecision(5) << m_dJDPassTimes[1439] << std::endl;

    // rotate Position vector from TEME (SGP4 output) to PEF (ECF)
    teme2pef(m_pnPos, gsto);

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
    double m_dLatGeod = 67;
    double m_dLonGeod = 21;
    double m_dAltitude = 0.4;       // km?
    
    gsCoord(m_dGsVecPef, m_dLatGeod, m_dLonGeod, m_dAltitude);
    
    for(int i = 0; i < 3; i++)
    {
        std::cout << " P_pef[" <<  i << "] = " << std::fixed << std::setprecision(4) << m_dGsVecPef[i];
     
    }
    std::cout << " " << std::endl;

    
    // Computation of terminal - satellite vector in PEF (ECF) coordinates
    double m_dGsSatVecPef[3] = {0};

    for(int i = 0; i < 3; i ++)
    {
        m_dGsSatVecPef[i] = m_dPosVec[0][i] - m_dGsVecPef[i];
    }
    
    double m_dGsSatVecSez[3];

    // Transformation of ground station - satellite vector to topocentric (SEZ) coordinates
    pef2sez(m_dGsSatVecSez, m_dGsSatVecPef, m_dLatGeod, m_dLonGeod);

    for(int i = 0; i < 3; i++)
    {
        std::cout << " Psez[" <<  i << "] = " << std::fixed << std::setprecision(4) << m_dGsSatVecSez[i] << std::endl;
     
    }

    double m_dRange;
    double m_dElev;
    double m_dAz;

    // Get angles and distance for gs-sat vector
    getAngles(m_dGsSatVecSez, m_dElev, m_dAz, m_dRange);

    std::cout << "Elevation: " << std::fixed << std::setprecision(4) << rad2deg(m_dElev) << std::endl;
    std::cout << "Azimuth: " << std::fixed << std::setprecision(4) << rad2deg(m_dAz) << std::endl;
    std::cout << "Range: " << std::fixed << std::setprecision(4) << m_dRange << std::endl;

    return;
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

void teme2pef(double *vec, double gsto)
{
    double rotMat[3][3];

    // Need to get Jdate

    rotMat[0][0] = cos(gsto);       rotMat[0][1] = sin(gsto);   rotMat[0][2] = 0;
    rotMat[1][0] = -1 * sin(gsto);  rotMat[1][1] = cos(gsto);   rotMat[1][2] = 0;
    rotMat[2][0] = 0;               rotMat[2][1] = 0;           rotMat[2][2] = 1;

    vec[0] = rotMat[0][0] * vec[0] + rotMat[0][1] * vec[1] + rotMat[0][2] * vec[2];
    vec[1] = rotMat[1][0] * vec[0] + rotMat[1][1] * vec[1] + rotMat[1][2] * vec[2];
    vec[2] = rotMat[2][0] * vec[0] + rotMat[2][1] * vec[1] + rotMat[2][2] * vec[2];
    

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

void printDate()
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


    std::time_t t = std::time(0);   // get time now
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
    double m_dLatGeoc = getGeocentricLatitude(dLatGeod);;
    std::cout << "Geocentric latitude = " << std::fixed << std::setprecision(3) << m_dLatGeoc << std::endl; 

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
    std::cout << "Range = " << std::fixed << std::setprecision(3) << dRange << std::endl; 

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

int main(void)
{
    int cnt = 0;
    VERIN *verins = NULL;

    //printDate();
    testImport();

    

    // cnt = readVERINs(&verins);

    // printf("read %d verins\n",cnt);
    // runVER(verins,cnt);

    // free(verins);

    return 0;
}
