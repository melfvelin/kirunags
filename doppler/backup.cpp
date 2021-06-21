BACKUP

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
        teme2pef(m_pnPos, gsto);

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

int passFinder()
{
    char satID[40];
    char lineOne[70];
    char lineTwo[70];
    FILE *in_file = NULL;
    char m_cOutputLine[256];
    TLE tle;
    //long time = mSecSince1970();
    long m_nPassTimes[1440];
    double m_dJDPassTimes[1440];
    double m_dPosVecTeme[1440][3];
    double m_dVelVecTeme[1440][3];
    double m_dPosVecPef[1440][3];
    double m_dPosVecSez[1440][3];
    double m_dGsSatVecPef[1440][3];
    double m_dGsSatVecSez[1440][3];
    double m_dJdayTLE = tle.rec.jdsatepoch + tle.rec.jdsatepochF;
    double m_dJdayNow;
    double m_dGsVecPef[3];
    double m_dLatGeod = deg2Rad(67.8404);
    double m_dLonGeod = deg2Rad(20.4105);
    double m_dAltitude = 0.4;       // km?

    double m_dRange[1440];
    double m_dElev[1440];
    double m_dAz[1440];

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
    
    // get sys time
    std::time_t timeObject = std::time(0);
    std::time_t tNow = timeObject;
    // get JDay from sys time
    m_dJdayNow = currentJdut1(&timeObject);

    // get GMST from Jday
    double gsto = gstime(m_dJdayNow);

    #ifdef debug
    std::cout << "GMST (deg) = " << std::fixed << std::setprecision(5) << rad2deg(gsto) << std::endl;
    std::cout << "Current JD = " << std::fixed << std::setprecision(8) << m_dJdayNow << std::endl;
    #endif /* debug */


    // Computation of gs position vector in PEF
    gsCoord(m_dGsVecPef, m_dLatGeod, m_dLonGeod, m_dAltitude);


    //std::time_t tNow = std::time(0);
    // This loop calculates state vectors for 2 days forward from NOW with 120 sec step
    for(int i = 0; i < 1440; i++)
    {
    
    
        timeObject = tNow + 2 * 60 * i;                   // will start at Tnow + 2 min and end at Tnow + 2 days
        m_nPassTimes[i] = timeObject * 1000;

        // Get Jdays and GMST for all time steps
        m_dJDPassTimes[i] = currentJdut1(&timeObject);
        gsto = gstime(m_dJDPassTimes[i]);

        // Get state vectors for satellite
        tle.TLE::getRVForDate(m_nPassTimes[i], m_dPosVecTeme[i], m_dVelVecTeme[i]);

        // transform TEME -> PEF
        teme2pef(m_dPosVecPef[i], m_dPosVecTeme[i], gsto);

        // Compute gs-satellite vectors in PEF
        for(int j = 0; j < 3; j ++)
        {
            m_dGsSatVecPef[i][j] = m_dPosVecPef[i][j] - m_dGsVecPef[j];
        }
        // Find a way to deal with passing arrays as arguments
        // Transformation of ground station - satellite vector to topocentric (SEZ) coordinates
        //m_pdTest = &m_dGsSatVecSez[i][0]; 
        pef2sez(m_dGsSatVecSez[i], m_dGsSatVecPef[i], m_dLatGeod, m_dLonGeod);

        getAngles(m_dGsSatVecSez[i], m_dElev[i], m_dAz[i], m_dRange[i]);
    }

    std::cout << "GsSatVecPef Sample " << std::fixed << std::setprecision(4) << m_dGsSatVecPef[300] << std::endl;
    std::cout << "Elevation Sample " << std::fixed << std::setprecision(4) << m_dElev[300] << std::endl;

    #ifdef debug
    // rotate Position vector from TEME (SGP4 output) to PEF (ECF)
    std::cout << "Elevation: " << std::fixed << std::setprecision(4) << rad2deg(m_dElev[0]) << std::endl;
    std::cout << "Azimuth: " << std::fixed << std::setprecision(4) << rad2deg(m_dAz[0]) << std::endl;
    std::cout << "Range: " << std::fixed << std::setprecision(4) << m_dRange[0] << std::endl;
    #endif /* debug */

    int elevcount = 0;

    for(int i = 0; i < 1439; i++)
    {
        if(m_dElev[i] > 0.0)
        {
            elevcount++;
        }
    }

    std::cout << "Number of times with positive elevation is: " << elevcount << std::endl;

    return 0;
} /* passFinder() */