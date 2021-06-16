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
    double m_dPosVec[1440][3];
    double m_dVelVec[1440][3];
    double *m_pnPos;
    double *m_pnVel;
    double m_dJdayTLE = tle.rec.jdsatepoch + tle.rec.jdsatepochF;
    double m_dJdayNow;
    double testTime = difftime(time, tle.epoch);

    // test

    double diff = (double)time - tle.epoch;
    diff/=60000.0;
    std::cout << "Minutes since epoch: "<< std::fixed << std::setprecision(3) << diff << std::endl;

    /*
    std::cout << "Epoch from TLE object: " << tle.epoch << std::endl;
    std::cout << "Time in milisec since 1970: " << time << std::endl;
    std::cout << "Hours between now and epoch: "<< std::fixed << std::setprecision(3) << testTime/86400 << std::endl;
    std::cout << "JDUT1: " << std::fixed << std::setprecision(5) << jdut1 << std::endl;
    std::cout << "jd epoch day: " << std::fixed << std::setprecision(5) << tle.rec.jdsatepoch << std::endl;
    std::cout << "jd epoch frac day: " << std::fixed << std::setprecision(5) << tle.rec.jdsatepochF << std::endl;
    std::cout << "combined jday double: " << std::fixed << std::setprecision(8) << m_dJdayTLE << std::endl;
    // make a time vector with 2 minute spacing and get RV vectors for each time
    */
    
    m_dJdayNow = currentJdut1();
    double gsto = gstime(m_dJdayNow);
    std::cout << "GMST (deg) = " << std::fixed << std::setprecision(5) << gsto*360/twopi << std::endl;
    // then add rotation of today? or is it already included?

    for(int i = 0; i < 1440; i++)
    {
        // insert jdates here?
        m_nPassTimes[i] = time + (2 * 60 * 1000 * i);
        m_pnPos = &m_dPosVec[i][0];
        m_pnVel = &m_dVelVec[i][0];

        tle.TLE::getRVForDate(m_nPassTimes[i], m_pnPos, m_pnVel);
    }

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
double currentJdut1()
{
    int m_nYear;
    int m_nMon;
    int m_nDay;
    int m_nHour;
    int m_nMin;
    double m_dSec;
    double m_dJday;
    double m_dJdayFrac;
    


    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::gmtime(&t);
    
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

int main(void)
{
    int cnt = 0;
    VERIN *verins = NULL;

    printDate();
    testImport();

    

    // cnt = readVERINs(&verins);

    // printf("read %d verins\n",cnt);
    // runVER(verins,cnt);

    // free(verins);

    return 0;
}
