#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iomanip>
#include "TLE.h"
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
    
    long time = mSecSince1970();
    long m_nPassTimes[1440];
    double m_dPosVec[1440][3];
    double m_dVelVec[1440][3];
    double *m_pnPos;
    double *m_pnVel;

    // make a time vector with 2 minute spacing and get RV vectors for each time
    for(int i = 0; i < 1440; i++)
    {
        m_nPassTimes[i] = time + (2 * 60 * 1000 * i);
        m_pnPos = &m_dPosVec[i][0];
        m_pnVel = &m_dVelVec[i][0];

        tle.TLE::getRVForDate(m_nPassTimes[i], m_pnPos, m_pnVel);
    }

    // Now we have r and v vectors in ECI and need to transform to SEZ to check against elevation
    // of our ground station to find passes

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


int main(void)
{
    int cnt = 0;
    VERIN *verins = NULL;

    testImport();

    

    // cnt = readVERINs(&verins);

    // printf("read %d verins\n",cnt);
    // runVER(verins,cnt);

    // free(verins);

    return 0;
}
