// orbitPrediction
#include "orbitPrediction.h"

// fix tle import
double *instantPredict(std::time_t timeObject)
{
	// Initialization of file handling variables
    char satID[40];
    char lineOne[70];
    char lineTwo[70];
    FILE *in_file = NULL;
    // Init of TLE object implemented by TLE.cpp
    TLE tle;

    // Init of time variables
    double m_dJdayNow;
    double m_dTimeStamp;

    /* Init of position vectors in all coordinate frames for t0 and t0+dt
        teme = true equator mean equinox, pef = pseudo earth fixed, sez = topocentric horizon */
    double m_dPosVecTeme[3] = {0};
    double m_dPosVecTemeDt[3] = {0};
    double m_dPosVecPef[3] = {0};
    double m_dPosVecPefDt[3] = {0};
    double m_dGsSatVecPef[3] = {0};
    double m_dGsSatVecPefDt[3] = {0};
    double m_dGsSatVecSez[3];
    double m_dGsSatVecSezDt[3];
    double m_dGsVecPef[3];

    /* Init of velocity vectors, these are not used as of now but can be used
        to compute Doppler shift analytically yielding higher accuracy      */
    double m_dVelVec[3] = {0};
    double m_dVelVecDt[3] = {0};
    // Init Dt, time between state vectors used for Doppler calculation
    double Dt = 1;
    
    // Init of GS latitude and longitude (geodetic) change gs position here
    double m_dLatGeod = deg2Rad(67.8404);
    double m_dLonGeod = deg2Rad(20.4105);
    double m_dAltitude = 0.4;       

    // Init variables for range and angels 
    double m_dRange;
    double m_dElev;
    double m_dAz;
    double m_dRangeDt;
    double m_dElevDt;
    double m_dAzDt;

    // double check before removing
    double m_dRetVals[4];

    // Init of return values
    double *m_pdRetVals = nullptr;
    m_pdRetVals = new double[5];
    
    // double gsto = gstime_SPG4(jdut1);

    #ifdef debug
        std::cout << "############## testImport() ###########" << std::endl;
    #endif /* debug */
    
    // Try using a string for file location
    in_file = fopen("tles/estcube1.TLE","r");

    if(in_file != NULL)
    {
        #ifdef debug
            std::cout << "File opened successfully" << std::endl;    
        #endif /* debug */

    }

    if ((fgets(satID, 39, in_file)) != NULL )
    {
        #ifdef debug
            std::cout << "Satellite ID read successfully" << std::endl;       
        #endif /* debug */        
    }
    
    if ((fgets(lineOne, 255, in_file)) != NULL )
    {
        #ifdef debug
            std::cout << "First line read successfully" << std::endl;       
        #endif /* debug */        
    }

    if ((fgets(lineTwo, 255, in_file)) != NULL )
    {
        #ifdef debug
            std::cout << "Second line read successfully" << std::endl;       
        #endif /* debug */
    }

    if(in_file)
    {
        fclose(in_file);
    }

    #ifdef debug
        std::cout << "Sat ID: " << satID << std::endl;
    #endif /* debug */

    // get orbital elements from TLE file 
    tle.parseLines(lineOne, lineTwo);
    
    // Get current Julian day and GMST angle
    m_dJdayNow = currentJdut1(&timeObject);
    double gsto = gstime(m_dJdayNow);

    #ifdef debug
        std::cout << "GMST (deg) = " << std::fixed << std::setprecision(5) << rad2deg(gsto) << std::endl;
        std::cout << "Current JD = " << std::fixed << std::setprecision(8) << m_dJdayNow << std::endl;
    #endif /* debug */

    // using the same gmst for delta t = 1 sec
    tle.TLE::getRVForDate(timeObject*1000, m_dPosVecTeme, m_dVelVec);
    tle.TLE::getRVForDate((timeObject*1000 + 1000 * Dt), m_dPosVecTemeDt, m_dVelVecDt);
    
    // TEME -> PEF transform for Position vectors
    teme2pef(m_dPosVecPef, m_dPosVecTeme, gsto);
    teme2pef(m_dPosVecPefDt, m_dPosVecTemeDt, gsto);

    // Compute ground station position vector in PEF
    gsCoord(m_dGsVecPef, m_dLatGeod, m_dLonGeod, m_dAltitude);

    
    // Computation of gs - satellite vector in PEF
    for(int i = 0; i < 3; i ++)
    {
        m_dGsSatVecPef[i] = m_dPosVecPef[i] - m_dGsVecPef[i];
        m_dGsSatVecPefDt[i] = m_dPosVecPefDt[i] - m_dGsVecPef[i];        
    }

    // Transformation of ground station - satellite vector to SEZ 
    pef2sez(m_dGsSatVecSez, m_dGsSatVecPef, m_dLatGeod, m_dLonGeod);
    pef2sez(m_dGsSatVecSezDt, m_dGsSatVecPefDt, m_dLatGeod, m_dLonGeod);

    // Get angles and distance for gs-sat vector
    getAngles(m_dGsSatVecSez, m_dElev, m_dAz, m_dRange);
    getAngles(m_dGsSatVecSezDt, m_dElevDt, m_dAzDt, m_dRangeDt);

    #ifdef debug
        std::cout << "Elevation: " << std::fixed << std::setprecision(4) << rad2deg(m_dElev) << std::endl;
        std::cout << "Azimuth: " << std::fixed << std::setprecision(4) << rad2deg(m_dAz) << std::endl;
        std::cout << "Range: " << std::fixed << std::setprecision(4) << m_dRange << std::endl;

        std::cout << "Doppler shift (100MHz): " << std::fixed << std::setprecision(3) << dopplerShift(m_dRange, m_dRangeDt, Dt) << std::endl;

        std::cout << "Azi before return: " << std::fixed << std::setprecision(3) << rad2deg(m_dAz) << std::endl;
        std::cout << "Timestamp: " << std::fixed << std::setprecision(3) << m_dRetVals[4] << std::endl;        
    #endif /* debug */
    

    m_pdRetVals[0] = rad2deg(m_dElev);
    std::cout << "Az (rad): " << std::fixed << std::setprecision(2) << m_dAz << std::endl;
    m_pdRetVals[1] = rad2deg(m_dAz);
    m_pdRetVals[2] = m_dRange;
    m_pdRetVals[3] = dopplerShift(m_dRange, m_dRangeDt, Dt);
    m_pdRetVals[4] = (double)timeObject;

    return m_pdRetVals;
}
